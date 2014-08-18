

#include "ns3/mp-tcp-id-manager-impl.h"
#include "ns3/log.h"
#include <algorithm>


namespace ns3
{

NS_LOG_COMPONENT_DEFINE("MpTcpPathIdManagerImpl");


MpTcpPathIdManagerImpl::MpTcpPathIdManagerImpl() :
  MpTcpPathIdManager()
{
  NS_LOG_INFO(this);
}

MpTcpPathIdManagerImpl::~MpTcpPathIdManagerImpl()
{
  NS_LOG_INFO(this);
}



//MpTcpAddressContainer::iterator
////uint8_t
//MpTcpPathIdManagerImpl::FindAddrIdOfAddr(Address addr )
//{
//
//  ;
//  for(MpTcpAddressContainer::iterator it = m_addrs.begin(); it = m_addrs.end() ; it ++)
//  {
//
//    if( it->second.first == addr)
//    {
//      NS_LOG_INFO("Found a match for addr " << addr << " : associated id is " << it->first)
////      return it->first;
//      return it;
//    }
//  }
//  return m_addrs.end();
//}



//
//uint8_t
bool
MpTcpPathIdManagerImpl::AddRemoteAddr(uint8_t addrId, const Ipv4Address& addressToRegister, uint16_t portToRegister)
{
  MpTcpAddressContainer& container = m_addrs;

//  MpTcpAddressInfo addrInfo = std::make_pair(address,port);


  NS_LOG_INFO("Trying to ADD_ADDR [" << addrId << "] ");

  MpTcpAddressContainer::iterator it = container.find( addrId );
  // if id already registered then we need to check the IPs are the same as the one advertised and that ports are different

  if(it != container.end() )
  {
    //
    Ipv4Address addrRegisteredWithId = it->second.first;
//    uint16_t portRegisteredTo)
//    for(MpTcpAddressContainer::iterator it2 = it.first; it2 != it.second; it2++)
//    {

      // id already registered to another IP
      if (addrRegisteredWithId != addressToRegister)
      {
        NS_LOG_WARN("Rejected ADD_ADDR because id [" << addrId
              << "] already registered with IP " << addressToRegister
              );
        return false;
      }

      std::vector<uint16_t>& ports = it->second.second;

      // if port specified
      if(portToRegister != 0 )
      {
        std::vector<uint16_t>::iterator it2 = std::find(ports.begin(), ports.end(), portToRegister);

        // if port already registered
        if( it2 != ports.end() )
        {
          NS_LOG_WARN("Rejected ADD_ADDR because this port " << portToRegister
                << " was already advertised with id [" << addrId << "] "
                );
          return false;
        }
        else {
          ports.push_back(portToRegister);
        }

      }
    }
    else
    {
      std::vector<uint16_t> ports;

      container.insert(
       std::make_pair(addrId,
                std::make_pair(addressToRegister, ports)
                )
              );
    }

// callback to know if we should accept it ?
//  }

  // assume it works

  // Should be RFC compliant to addaddr
//  NotifyAddAddr( addrInfo );
  return true;
}


//uint8_t
//MpTcpPathIdManagerImpl::GenerateAddrId(MpTcpAddressInfo)
//{
//
//}


bool
MpTcpPathIdManagerImpl::RemRemoteAddr(uint8_t addrId)
{
//  MpTcpAddressContainer& container = m_remoteAddrs[remote];

  // TODO retreive the address, check there is no subflow establish with this ID
  // Than remove it from available addresses
  NS_LOG_ERROR("Function not implemented ");

      MpTcpAddressContainer::size_type res = m_addrs.erase( addrId );
//  NotifyRemAddress(addrId);
  return (res != 0);
}


uint8_t
MpTcpPathIdManagerImpl::GetLocalAddrId(const InetSocketAddress& address)
{
  //TODO should be able to improve AddrId allocation to allow for more choices
  // converts Static into member function ? add a modulo in case we add too many local addr ?
  static uint8_t addrId = 0;
  addrId++;

  // TODO check if it's owned by the node ? or not ?

  std::pair< std::map<Ipv4Address,uint8_t>::iterator , bool > result = m_localAddresses.insert( std::make_pair(address, addrId) );
//  std::map<Ipv4Address,uint8_t>::iterator it = m_localAddresses.find( address );
//  if( ! result.second)
//  {
    return result.first->second;
//  }
//  return
}



bool
MpTcpPathIdManagerImpl::RemLocalAddr(Ipv4Address address, uint8_t& id)
{
  std::map<Ipv4Address,uint8_t>::iterator it  = m_localAddresses.find(address);
  if(it != m_localAddresses.end() )
  {
    id = it->second;
    return true;
  }
  return false;
}

void
MpTcpPathIdManagerImpl::GetAllAdvertisedDestinations(std::vector<InetSocketAddress>& addresses)
{
  addresses.clear();
  for(MpTcpAddressContainer::iterator externIt = m_addrs.begin(); externIt != m_addrs.end(); externIt++)
  {
    std::vector<uint16_t>& portList = externIt->second.second;

    for( std::vector<uint16_t>::iterator internIt = portList.begin(); internIt != portList.end() ;internIt++ )
    {
      // ip / port
      addresses.push_back( InetSocketAddress( externIt->second.first, *internIt) );
    }
  }
}


#if 0
void
MpTcpSocketBase::SetAddAddrCallback(Callback<bool, Ptr<Socket>, Address, uint8_t> addAddr)
{
  NS_LOG_FUNCTION (this << &addAddr);

  m_onAddAddr = addAddr;
}

void
MpTcpSocketBase::NotifyAddAddr(MpTcpAddressInfo info)
{
  if (!m_onAddAddr.IsNull())
  {
    // TODO user should not have to deal with MpTcpAddressInfo
    m_onAddAddr (this, info.first, info.second);
  }
}

#endif

} // namespace ns3