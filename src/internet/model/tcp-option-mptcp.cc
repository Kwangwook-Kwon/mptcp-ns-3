/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Adrian Sai-wah Tam
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Adrian Sai-wah Tam <adrian.sw.tam@gmail.com>
 */

#include "tcp-option-mptcp.h"
#include "ns3/log.h"


/**
\note This is a global MPTCP option logger
*/
NS_LOG_COMPONENT_DEFINE("TcpOptionMpTcp");
//NS_LOG_COMPONENT_DEFINE("TcpOptionMpTcpCapable");

namespace ns3 {
//
//NS_OBJECT_ENSURE_REGISTERED (TcpOptionMpTcpCapable);
//NS_OBJECT_ENSURE_REGISTERED (TcpOptionMpTcpAddAddress );
//NS_OBJECT_ENSURE_REGISTERED (TcpOptionMpTcpRemoveAddress );
//NS_OBJECT_ENSURE_REGISTERED (TcpOptionMpTcpJoinInitialSyn );
NS_OBJECT_ENSURE_REGISTERED (TcpOptionMpTcpChangePriority );
NS_OBJECT_ENSURE_REGISTERED (TcpOptionMpTcpMain );
NS_OBJECT_ENSURE_REGISTERED (TcpOptionMpTcp<DSS> );


/////////////////////////////////////////////////////////
////////  Base for MPTCP options
/////////////////////////////////////////////////////////
TcpOptionMpTcpMain::TcpOptionMpTcpMain() :
  TcpOption()
{
  NS_LOG_FUNCTION(this);
}


TcpOptionMpTcpMain::~TcpOptionMpTcpMain()
{
  NS_LOG_FUNCTION(this);
}

TypeId
TcpOptionMpTcpMain::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpOptionMpTcpMain")
    .SetParent<TcpOption> ()
//    .AddConstructor<TcpOptionMpTcp> ()
  ;
  return tid;
}


TypeId
TcpOptionMpTcpMain::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


Ptr<TcpOption>
TcpOptionMpTcpMain::CreateOption(uint8_t subtype)
{
  switch(subtype)
  {
    case MP_CAPABLE:
//      return CreateObject<TcpOptionMpTcpCapable>();

    default:
      NS_ASSERT_MSG(false,"Unsupported MPTCP suboption");
  };

  return 0;
}

void
TcpOptionMpTcpMain::SerializeRef (Buffer::Iterator& i) const
{
//    Buffer::Iterator& i = start;
    i.WriteU8 (GetKind ()); // Kind
    i.WriteU8 ( GetSerializedSize() ); // Length


    // TODO may be an error otherwise here !

//    i.WriteU8 ( ( (GetSubType() << 4 ) && 0xf0) ); // Subtype TODO should write U4 only
    //i.WriteU8 ( GetSerializedSize() ); // Subtype TODO should write U4 only

}



/////////////////////////////////////////////////////////
////////  MP_CAPABLE
/////////////////////////////////////////////////////////
TcpOptionMpTcpCapable::TcpOptionMpTcpCapable()
    : TcpOptionMpTcp (),
    m_version(0),
    m_flags( HMAC_SHA1 ),
    m_senderKey(0),
    m_remoteKey(0),
    m_length(12)
{
  NS_LOG_FUNCTION(this);
}

TcpOptionMpTcpCapable::~TcpOptionMpTcpCapable ()
{
  NS_LOG_FUNCTION_NOARGS();
}


bool
TcpOptionMpTcpCapable::operator==(const TcpOptionMpTcpCapable& opt) const
{
  return (GetPeerKey() == opt.GetPeerKey() && GetLocalKey() == opt.GetLocalKey() );
}


void
TcpOptionMpTcpCapable::SetSenderKey(uint64_t senderKey)
{
  NS_LOG_FUNCTION(this);
  m_senderKey = senderKey;
}

void
TcpOptionMpTcpCapable::SetRemoteKey(uint64_t remoteKey)
{
  NS_LOG_FUNCTION(this);
  m_length = 20;
  m_remoteKey = remoteKey;
}

void
TcpOptionMpTcpCapable::Print (std::ostream &os) const
{
  os << "MP_CAPABLE. version" << m_version
    << "Flags:" << m_flags
    << "Sender's Key [" << GetLocalKey() << "]";
  if( HasPeerKey() )
  {
    os  << "Peer's Key [" << GetPeerKey() << "]";
  }
}

bool
TcpOptionMpTcpCapable::IsChecksumRequired() const
{
  return ( m_flags >> 7);
}

void
TcpOptionMpTcpCapable::Serialize (Buffer::Iterator i) const
{
//  Buffer::Iterator i = start;
  TcpOptionMpTcp::SerializeRef(i);

  i.WriteU8 ( (GetSubType () << 4) + (0x0f & GetVersion()) ); // Kind
  i.WriteU8 ( m_flags ); // Length
  i.WriteHtonU64( GetLocalKey() );
  if( HasPeerKey() )
  {
    i.WriteHtonU64( GetPeerKey() );
  }
}

uint32_t
TcpOptionMpTcpCapable::Deserialize (Buffer::Iterator i)
{

  uint32_t length =  (uint32_t)i.ReadU8( );

  NS_ASSERT( length == 12 || length == 20 );
  //NS_ABORT_UNLESS

  uint8_t subtype_and_version = i.ReadU8();
  NS_ASSERT( subtype_and_version >> 4 == GetSubType()  );
  m_flags = i.ReadU8();

  SetSenderKey( i.ReadNtohU64() );

  if( length == 20)
  {
    SetRemoteKey( i.ReadNtohU64() );
  }
  return length;
}

uint32_t
TcpOptionMpTcpCapable::GetSerializedSize (void) const
{
  // 12 or 20
    return (m_length);
}




/////////////////////////////////////////////////////////
////////  MP_JOIN Initial SYN
/////////////////////////////////////////////////////////
TcpOptionMpTcpJoinInitialSyn::TcpOptionMpTcpJoinInitialSyn()
    : TcpOptionMpTcp (),
    m_addressId(0),
    m_flags(0),
    m_peerToken(0),
    m_nonce(0)
{
  NS_LOG_FUNCTION(this);

  // TODO Generate a random number
  m_nonce = 3232;
}

TcpOptionMpTcpJoinInitialSyn::~TcpOptionMpTcpJoinInitialSyn ()
{
  NS_LOG_FUNCTION_NOARGS();
}


void
TcpOptionMpTcpJoinInitialSyn::Print (std::ostream &os) const
{
  os << "MP_Join Initial Syn" << ";";
}


bool
TcpOptionMpTcpJoinInitialSyn::operator==(const TcpOptionMpTcpJoinInitialSyn& opt) const
{
  return (
    GetPeerToken() == opt.GetPeerToken()
    && m_nonce == opt.m_nonce
//    && GetLocalToken() == opt.GetLocalToken()
    && GetAddressId() == opt.GetAddressId()
    );
}


void
TcpOptionMpTcpJoinInitialSyn::Serialize (Buffer::Iterator i) const
{
  TcpOptionMpTcp::SerializeRef(i);
  i.WriteU8( GetSubType() << 4 );
  i.WriteU8( GetAddressId() );
  i.WriteHtonU32( GetPeerToken() );
  i.WriteHtonU32( m_nonce );
  // la je continue a sérialiser
}

uint32_t
TcpOptionMpTcpJoinInitialSyn::Deserialize (Buffer::Iterator i)
{
//  TcpOptionMpTcp::Deserialize(start);
  uint32_t length = (uint32_t) i.ReadU8();
  NS_ASSERT( length == 12);

  uint8_t subtype_and_flags = i.ReadU8()  ;
  NS_ASSERT( (subtype_and_flags >> 4) == GetSubType() );

  SetAddressId( i.ReadU8());
  SetPeerToken( i.ReadNtohU32() );
  m_nonce = i.ReadNtohU32();
  return 12;
}

// OK
uint32_t
TcpOptionMpTcpJoinInitialSyn::GetSerializedSize (void) const
{
    return 12;
}




///////////////////////////////////////:
//// MP_JOIN SYN_ACK
////
TcpOptionMpTcpJoinSynReceived::TcpOptionMpTcpJoinSynReceived()
    : TcpOptionMpTcp (),
    m_addressId(0),
    m_flags(0),
    m_truncatedHmac(0),
    m_nonce(0)
{
  NS_LOG_FUNCTION(this);
}

TcpOptionMpTcpJoinSynReceived::~TcpOptionMpTcpJoinSynReceived ()
{
  NS_LOG_FUNCTION_NOARGS();
}


void
TcpOptionMpTcpJoinSynReceived::Print (std::ostream &os) const
{
  os << "MP_Join Initial Syn" << ";" << m_addressId;
}

bool
TcpOptionMpTcpJoinSynReceived::operator==(const TcpOptionMpTcpJoinSynReceived& opt) const
{
  return (
      GetNonce() == opt.GetNonce()
      && GetTruncatedHmac() == opt.GetTruncatedHmac()
      && GetAddressId()  == opt.GetAddressId()
      )
      ;
}

void
TcpOptionMpTcpJoinSynReceived::SetTruncatedHmac(uint64_t hmac)
{
  // TODO truncate from real hmac ?
  m_truncatedHmac = hmac;
}

void
TcpOptionMpTcpJoinSynReceived::Serialize (Buffer::Iterator i) const
{
  TcpOptionMpTcp::SerializeRef(i);
  i.WriteU8( GetSubType() << 4 );
  i.WriteU8( GetAddressId() );

  i.WriteHtonU64( GetTruncatedHmac() );
  i.WriteHtonU32( m_nonce );

  // la je continue a sérialiser
}



uint32_t
TcpOptionMpTcpJoinSynReceived::Deserialize (Buffer::Iterator i)
{
  uint32_t length = (uint32_t) i.ReadU8();
  NS_ASSERT( length == 16);

  uint8_t subtype_and_flags = i.ReadU8()  ;
  NS_ASSERT( (subtype_and_flags >> 4) == GetSubType() );

  SetAddressId( i.ReadU8() );
  m_truncatedHmac = i.ReadNtohU64();

  m_nonce = i.ReadNtohU32();

  return length;
}

uint32_t
TcpOptionMpTcpJoinSynReceived::GetSerializedSize (void) const
{
    return 16;
}

///////////////////////////////////////:
//// MP_JOIN SYN_ACK
////
TcpOptionMpTcpJoinSynAckReceived::TcpOptionMpTcpJoinSynAckReceived()
    : TcpOptionMpTcp ()
//    m_(0),
//    m_receiverToken(0),
//    m_senderToken(0)
{
  NS_LOG_FUNCTION(this);
}

TcpOptionMpTcpJoinSynAckReceived::~TcpOptionMpTcpJoinSynAckReceived ()
{
  NS_LOG_FUNCTION(this); // Use this ?
}

bool
TcpOptionMpTcpJoinSynAckReceived::operator==(const TcpOptionMpTcpJoinSynAckReceived&) const
{
  // TODO compare 2 arrays
  return true;
}

void
TcpOptionMpTcpJoinSynAckReceived::Print (std::ostream &os) const
{
  os << "MP_Join SynAck received";
}

void
TcpOptionMpTcpJoinSynAckReceived::Serialize (Buffer::Iterator i) const
{
//  TcpOptionMpTcp::SerializeRef(start);
  TcpOptionMpTcp::SerializeRef(i);
  i.WriteU8( GetSubType() << 4 );
  i.WriteU8( 0 ); // Reserved

  // TODO write sender's HMAC
  i.Write(&m_hmac[0],160);
//  i.WriteHtonU64( 0  );
//  i.WriteHtonU64( 0  );
//  i.WriteHtonU32( 0 );

}

uint32_t
TcpOptionMpTcpJoinSynAckReceived::Deserialize (Buffer::Iterator i)
{
  uint32_t length =  (uint32_t)i.ReadU8( );

  NS_ASSERT( length == 24 );
  //NS_ABORT_UNLESS

  uint8_t subtype_and_rsvd = i.ReadU8();
  NS_ASSERT( subtype_and_rsvd >> 4 == GetSubType()  );
  i.ReadU8(); // reserved

  // Retrieve HMAC
  i.Read( &m_hmac[0],160);
//  i.ReadNtohU64();
//  i.ReadNtohU64();
//  i.ReadNtohU32();

  return length;
}

uint32_t
TcpOptionMpTcpJoinSynAckReceived::GetSerializedSize (void) const
{
    return 24;
}





///////////////////////////////////////:
//// MP_JOIN SYN_ACK
////
TcpOptionMpTcpDSN::TcpOptionMpTcpDSN() :
  TcpOptionMpTcp(),
  m_flags(0),
  m_dataAck(0)
//  ,m_dataSequenceNumber(0),
//  m_subflowSequenceNumber(0),
//  m_dataLevelLength(0)
{
  NS_LOG_FUNCTION(this);
}


TcpOptionMpTcpDSN::~TcpOptionMpTcpDSN()
{
  NS_LOG_FUNCTION_NOARGS();
}

void
TcpOptionMpTcpDSN::SetMapping(MpTcpMapping mapping)
{
  m_mapping = mapping;
  m_flags |= DSNMappingPresent;
//  m_flags |= DSNMappingPresent;
}

MpTcpMapping
TcpOptionMpTcpDSN::GetMapping( ) const
{
  return m_mapping;
}

uint32_t
TcpOptionMpTcpDSN::GetSerializedSize() const
{
  uint32_t length = 4;
//  4 + 4 + 4 +2 +2;

  if( m_flags & DataAckPresent)
  {
    length += 4;
    if( m_flags & DataAckOf8Bytes)  length += 4;

  }

  if( m_flags & DSNMappingPresent)
  {
    length += 12;
    if( m_flags & DSNOfEightBytes)  length += 4;
  }

  NS_LOG_UNCOND( "size " << length);

  return length;
}

void
TcpOptionMpTcpDSN::Print(std::ostream& os) const
{
  os << "MPTCP option DSN mapping. flags" << m_flags
//      << ""
      ;
}


void
TcpOptionMpTcpDSN::Serialize (Buffer::Iterator i) const
{
  //
  TcpOptionMpTcp::SerializeRef(i);
  i.WriteU8( GetSubType() << 4);
  i.WriteU8( m_flags );

  NS_LOG_INFO ("Serializing flags " <<  (int)m_flags);

  if( m_flags & DataAckPresent)
  {
    NS_LOG_INFO("Serializing DataAck");
    if( m_flags & DataAckOf8Bytes)
    {
      // Not implemented
    }
    else
    {
      i.WriteHtonU32( m_dataAck );
    }
  }

  if( m_flags & DSNMappingPresent)
  {
    NS_LOG_INFO("Serializing DSN mapping");
    if( m_flags & DSNOfEightBytes)
    {
      // Not implemented
    }
    else
    {
      i.WriteHtonU32( m_mapping.GetDataSequenceNumber().GetValue() );
    }
    i.WriteHtonU32( GetMapping().GetSubflowSequenceNumber().GetValue() );
    i.WriteHtonU16( GetMapping().GetDataLevelLength() );
    i.WriteHtonU16( 0 );  // Checksum
  }

//  i.WriteHtonU64( GetMapping().GetDataSequenceNumber().GetValue() );

}


uint32_t
TcpOptionMpTcpDSN::Deserialize (Buffer::Iterator i)
{
  uint32_t length =  (uint32_t)i.ReadU8( );


  // 4
  // +4   or  + 8
  // +12 or + 16
  NS_ASSERT( (length % 4) == 0 && length <= 28);
//    length == 4 // if it's 4, it doesn't carry anything :/
//    || length== 8 || length == 12   // Only carries DataAck
//    || length == 16 || length == 20 // Only carries DSN mapping
//    || length ==
//    26


  uint8_t subtype_and_reserved = i.ReadU8();
//  NS_LOG_UNCOND("subtype " << (int)subtype_and_reserved << "compared to REAL one" << (int)GetSubType() );
  NS_ASSERT( (subtype_and_reserved >> 4) == GetSubType()  );
  m_flags = i.ReadU8();

  NS_LOG_INFO ("Deserialized flags " << (int)m_flags);
  if( m_flags & DataAckPresent)
  {
    NS_LOG_INFO("Deserializing DataAck");
    if( m_flags & DataAckOf8Bytes)
    {
      // Not implemented
    }
    else
    {
      m_dataAck = i.ReadNtohU32 ( );
    }
  }

  // Read mapping
  if(m_flags & DSNMappingPresent)
  {
    NS_LOG_INFO("Deserializing DSN mapping");

    uint32_t dataSeqNb(0);
    uint16_t dataLevelLength(0);

    if( m_flags & DSNOfEightBytes)
    {
      // Not implemented
    }
    else
    {
       dataSeqNb = i.ReadNtohU32( );
    }
    m_mapping.MapToSubflowSeqNumber( SequenceNumber32(i.ReadNtohU32())  );

    dataLevelLength = i.ReadNtohU16();

    m_mapping.Configure( SequenceNumber32(dataSeqNb ), dataLevelLength);
  }



  return length;

}

void
TcpOptionMpTcpDSN::SetDataAck(uint32_t dataAck)
{
  m_dataAck = dataAck;
  m_flags |= DataAckPresent;
}

bool
TcpOptionMpTcpDSN::operator==(const TcpOptionMpTcpDSN& opt) const
{
  //!
  return (m_flags == opt.m_flags
      && GetMapping() == opt.GetMapping()
      && GetDataAck() == opt.GetDataAck()
    );
}

//void
//TcpOptionMpTcpDSN::Configure(uint64_t dataSeqNb, uint32_t subflowSeqNb, uint16_t dataLength)
//{
//  m_dataSequenceNumber = dataSeqNb;
//  m_subflowSequenceNumber = subflowSeqNb;
//  m_dataLevelLength = dataLength;
//}




///////////////////////////////////////:
//// ADD_ADDR
////
TcpOptionMpTcpAddAddress::TcpOptionMpTcpAddAddress() :
  TcpOptionMpTcp(),
//  m_length(4),
  m_addressVersion(0),
  m_addrId(0),
  m_port(0)
{
  NS_LOG_FUNCTION(this);
}

TcpOptionMpTcpAddAddress::~TcpOptionMpTcpAddAddress()
{
  NS_LOG_FUNCTION_NOARGS();
}

void
TcpOptionMpTcpAddAddress::SetAddress(InetSocketAddress address, uint8_t addrId)
{
  m_address = address.GetIpv4();
  m_port = address.GetPort();
  m_addrId  = addrId;
//  m_length = 10;
  m_addressVersion = 4;
}

//void
//TcpOptionMpTcpAddAddress::SetAddress(Ipv6Address address)
//{
//  m_address6 = address;
//  m_addressVersion = 6;
////  m_length = 26;
//}


void
TcpOptionMpTcpAddAddress::Print (std::ostream &os) const
{
  os << "MP ADD_ADDR: " << m_addrId;
//  if( GetSerializedSize() == 4)
//  {
//    os << addrId;
//  }
}

void
TcpOptionMpTcpAddAddress::GetAddress( InetSocketAddress& address) const
{
  address.SetIpv4( m_address);
  address.SetPort( m_port );

}

//virtual uint8_t GetAddress(Address& address) const;

//void
//TcpOptionMpTcpAddAddress::SetAddressId(uint8_t addrId)
//{
//  m_addrId = addrId;
////  m_length = 4;
//}

uint8_t
TcpOptionMpTcpAddAddress::GetAddressId() const
{
  return m_addrId;
}

void
TcpOptionMpTcpAddAddress::Serialize (Buffer::Iterator i) const
{
//  Buffer::Iterator i = start;
  TcpOptionMpTcp::SerializeRef(i);


  i.WriteU8( (GetSubType() << 4) + (uint8_t) m_addressVersion );
  i.WriteU8( m_addrId );

  if(m_addressVersion == 4)
  {
//    i.WriteHtonU32( m_address.GetIpv4().Get() );
    i.WriteHtonU32( m_address.Get() );
  }
  else
  {
    NS_ASSERT_MSG(m_addressVersion==6, "You should set an IP address before serializing MPTCP option ADD_ADDR");

    uint8_t  	buf[16];
//    m_address6.GetIpv6().GetBytes( buf );
    m_address6.GetBytes( buf );
    for(int j = 0; j < 16; ++j)
    {
      i.WriteU8( buf[j] );
    }
    NS_LOG_INFO(this <<  "Got bytes from ipv6");
  }

  i.WriteU8(m_port);
}


//bool
//TcpOptionMpTcpAddAddress::EmbeddedAddressId()
//{
//  return ( GetSerializedSize() == 4);
//}

uint32_t
TcpOptionMpTcpAddAddress::Deserialize (Buffer::Iterator i)
{
  uint32_t length =  (uint32_t)i.ReadU8( );

  NS_ASSERT( length == 10
//      || length == 8
//      || length == 20
      || length==22
      );
  //NS_ABORT_UNLESS

  uint8_t subtype_and_ipversion = i.ReadU8();
  NS_ASSERT( subtype_and_ipversion >> 4 == GetSubType()  );

  m_addressVersion = subtype_and_ipversion  & 0x0f;
  NS_ASSERT_MSG(m_addressVersion == 4 || m_addressVersion == 6, "Unsupported address version");

  m_addrId =  i.ReadU8();

//  SetSenderKey( i.ReadNtohU64() );

  if( m_addressVersion == 4)
  {
    m_address.Set ( i.ReadNtohU32() );
  }
  else
  {
    // ipv6
//    m_address6.Set( );
  }
  return length;
}


uint32_t
TcpOptionMpTcpAddAddress::GetSerializedSize (void) const
{
  if(m_addressVersion == 4)
  {
    return  10;
  }

  NS_ASSERT_MSG(m_addressVersion == 6,"Wrong IP version. Maybe you didn't set an address to the MPTCP ADD_ADDR option ?");
  return 22;
}


bool
TcpOptionMpTcpAddAddress::operator==(const TcpOptionMpTcpAddAddress& opt) const
{
  return (GetAddressId() == opt.GetAddressId()
    && m_address == opt.m_address
    && m_address6 == opt.m_address6
    && m_port == opt.m_port

  );
}


///////////////////////////////////////:
//// DEL_ADDR change priority
////

TcpOptionMpTcpRemoveAddress::TcpOptionMpTcpRemoveAddress() :
  TcpOptionMpTcp()
//  ,m_addrId(0)
{
  //
  NS_LOG_FUNCTION(this);
}


TcpOptionMpTcpRemoveAddress::~TcpOptionMpTcpRemoveAddress()
{
  //
  NS_LOG_FUNCTION_NOARGS();
}


void
TcpOptionMpTcpRemoveAddress::GetAddresses(std::vector<uint8_t>& addresses)
{
  addresses = m_addressesId;
}


void
TcpOptionMpTcpRemoveAddress::AddAddressId( uint8_t addrId )
{
  m_addressesId.push_back( addrId );
}


void
TcpOptionMpTcpRemoveAddress::Serialize (Buffer::Iterator i) const
{
//  Buffer::Iterator i = start;
  TcpOptionMpTcp::SerializeRef(i);

  i.WriteU8( (GetSubType() << 4) );
  for(
      std::vector<uint8_t>::const_iterator it = m_addressesId.begin();
        it != m_addressesId.end();
        it++
        )
  {
    i.WriteU8( *it );
  }
}

uint32_t
TcpOptionMpTcpRemoveAddress::Deserialize (Buffer::Iterator i)
{
  uint32_t length =  (uint32_t)i.ReadU8( );

  NS_ASSERT_MSG( length > 3,"You probably forgot to add AddrId to the MPTCP Remove option");
  //NS_ABORT_UNLESS

  uint8_t subtype_and_resvd = i.ReadU8();
  NS_ASSERT( subtype_and_resvd >> 4 == GetSubType()  );
//  m_flags = i.ReadU8();
//
//  SetSenderKey( i.ReadNtohU64() );
  m_addressesId.clear();
  for(uint32_t j = 3; j < length; ++j)
  {
    m_addressesId.push_back( i.ReadU8() );
  }

  return length;

}

uint32_t
TcpOptionMpTcpRemoveAddress::GetSerializedSize (void) const
{
  return ( 3 + m_addressesId.size() );
}

void
TcpOptionMpTcpRemoveAddress::Print (std::ostream &os) const
{
  os << "REMOVE_ADDR option ";

}

bool
TcpOptionMpTcpRemoveAddress::operator==(const TcpOptionMpTcpRemoveAddress& opt) const
{
  return (m_addressesId == opt.m_addressesId);

}


///////////////////////////////////////:
//// MP_PRIO change priority
////
TcpOptionMpTcpChangePriority::TcpOptionMpTcpChangePriority() :
  TcpOptionMpTcp(),
  m_length(3),
  m_addrId(0),
  m_backupFlag(false)
{
}

void
TcpOptionMpTcpChangePriority::Print (std::ostream &os) const
{
  os << "MP_Prio: Change priority to " << m_backupFlag;
  if( GetSerializedSize() == 4)
  {
    os << m_addrId;
  }
}

void
TcpOptionMpTcpChangePriority::SetAddressId(uint8_t addrId)
{
  m_addrId = addrId;
  m_length = 4;
}

bool
TcpOptionMpTcpChangePriority::GetAddressId(uint8_t& addrId) const
{
  if( !EmbeddedAddressId() )
    return false;

  addrId = m_addrId;
  return true;
}

void
TcpOptionMpTcpChangePriority::Serialize (Buffer::Iterator i) const
{
  NS_LOG_INFO("Serializing");
//  Buffer::Iterator i = start;
  TcpOptionMpTcp::SerializeRef(i);

  i.WriteU8( (GetSubType() << 4) + (uint8_t)m_backupFlag );
  if( EmbeddedAddressId() )
    i.WriteU8( m_addrId );
}


uint32_t
TcpOptionMpTcpChangePriority::Deserialize (Buffer::Iterator i)
{
  NS_LOG_INFO("Deserializing");

  uint32_t length =  (uint32_t)i.ReadU8( );

  NS_ASSERT( length == 3 || length == 4 );
  //NS_ABORT_UNLESS

  uint8_t subtype_and_backup = i.ReadU8();
  NS_ASSERT( subtype_and_backup >> 4 == GetSubType()  );
  m_backupFlag = subtype_and_backup & 0x0f;

  if( length == 4)
  {
    SetAddressId( i.ReadU8() );
  }

  return m_length;
}

uint32_t
TcpOptionMpTcpChangePriority::GetSerializedSize (void) const
{
  return m_length;
}



TypeId
TcpOptionMpTcpChangePriority::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpOptionMpTcpChangePriority")
//    .SetParent<TcpOptionMpTcpMain> ()
    .SetParent<TcpOptionMpTcpMain> ()
//    .AddConstructor<TcpOptionMpTcpMain> ()
  ;
  return tid;
}

TypeId
TcpOptionMpTcpChangePriority::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


bool
TcpOptionMpTcpChangePriority::EmbeddedAddressId() const
{
  return ( GetSerializedSize() == 4);
}

bool
TcpOptionMpTcpChangePriority::operator==(const TcpOptionMpTcpChangePriority& opt) const
{

  return (
    GetPriority() == opt.GetPriority()
//    && GetAddressId() ==
    && m_addrId == opt.m_addrId
    );
}



} // namespace ns3
