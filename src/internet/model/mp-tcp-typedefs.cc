#include <iostream>
#include "ns3/mp-tcp-typedefs.h"
#include "ns3/simulator.h"
#include "ns3/log.h"


NS_LOG_COMPONENT_DEFINE("MpTcpTypeDefs");

namespace ns3
{

MpTcpMapping::MpTcpMapping() :
  m_dataSequenceNumber(0),
  m_subflowSequenceNumber(0),
  m_dataLevelLength(0)
{
  NS_LOG_FUNCTION(this);
}

bool
MpTcpMapping::operator==( const MpTcpMapping& mapping) const
{
  //!
  return (
    GetDataLevelLength() == mapping.GetDataLevelLength()
    && GetDataSequenceNumber() == mapping.GetDataSequenceNumber()
    && GetDataLevelLength()  == GetDataLevelLength()
    );
}


//SequenceNumber32 subflowSeqNb
void
MpTcpMapping::Configure( SequenceNumber32  dataSeqNb, uint16_t mappingSize)
//  m_dataSeqNumber(dataSeqNb),
//  m_size(mappingSize)
{
  m_dataSequenceNumber = dataSeqNb;
  m_dataLevelLength = mappingSize;
}



#if 0
DSNMapping::DSNMapping(
  uint8_t sFlowIdx
  , uint64_t dSeqNum, uint16_t dLvlLen, uint32_t sflowSeqNum,
  uint32_t ack,
  Ptr<Packet> pkt
  )
{
  subflowIndex = sFlowIdx;
  dataSeqNumber = dSeqNum;
  dataLevelLength = dLvlLen;
  subflowSeqNumber = sflowSeqNum;
  acknowledgement = ack;
  dupAckCount = 0;
  packet = new uint8_t[dLvlLen];
  pkt->CopyData(packet, dLvlLen);
}
/*
 DSNMapping::DSNMapping (const DSNMapping &res)
 {
 subflowIndex     = res.subflowIndex;
 acknowledgement  = res.acknowledgement;
 dataSeqNumber    = res.dataSeqNumber;
 dataLevelLength  = res.dataLevelLength;
 subflowSeqNumber = res.subflowSeqNumber;
 dupAckCount      = res.dupAckCount;
 packet           = res.packet;
 original         = false;
 }
 */
DSNMapping::~DSNMapping()
{
  dataSeqNumber = 0;
  dataLevelLength = 0;
  subflowSeqNumber = 0;
  dupAckCount = 0;
  if (packet != 0)
    delete[] packet;
}

bool
DSNMapping::operator <(const DSNMapping& rhs) const
{
  return this->dataSeqNumber < rhs.dataSeqNumber;
}




DataBuffer::DataBuffer()
{
  bufMaxSize = 0;
}

DataBuffer::DataBuffer(uint32_t size)
{
  bufMaxSize = size;
}

DataBuffer::~DataBuffer()
{
  bufMaxSize = 0;
}

uint32_t
DataBuffer::Add(uint8_t* buf, uint32_t size)
{
  // read data from buf and insert it into the DataBuffer instance
  NS_LOG_FUNCTION (this << (int) size << (int) (bufMaxSize - (uint32_t) buffer.size()) );
  uint32_t toWrite = std::min(size, (bufMaxSize - (uint32_t) buffer.size()));

  if (buffer.empty() == true)
    {
      NS_LOG_INFO("DataBuffer::Add -> buffer is empty !");
    }
  else
    NS_LOG_INFO("DataBuffer::Add -> buffer was not empty !");

  uint32_t qty = 0;

  while (qty < toWrite)
    {
      buffer.push(buf[qty]);
      qty++;
    }NS_LOG_INFO("DataBuffer::Add -> amount of data = "<< qty);NS_LOG_INFO("DataBuffer::Add -> freeSpace Size = "<< (bufMaxSize - (uint32_t) buffer.size()) );
  return qty;
}

uint32_t
DataBuffer::Retrieve(uint8_t* buf, uint32_t size)
{
  NS_LOG_FUNCTION (this << (int) size << (int) (bufMaxSize - (uint32_t) buffer.size()) );
  uint32_t quantity = std::min(size, (uint32_t) buffer.size());
  if (quantity == 0)
    {
      NS_LOG_INFO("DataBuffer::Retrieve -> No data to read from buffer reception !");
      return 0;
    }

  for (uint32_t i = 0; i < quantity; i++)
    {
      buf[i] = buffer.front();
      buffer.pop();
    }

  NS_LOG_INFO("DataBuffer::Retrieve -> freeSpaceSize == "<< bufMaxSize - (uint32_t) buffer.size() );
  return quantity;
}

Ptr<Packet>
DataBuffer::CreatePacket(uint32_t size)
{
  NS_LOG_FUNCTION (this << (int) size << (int) ( bufMaxSize - (uint32_t) buffer.size()) );
  uint32_t quantity = std::min(size, (uint32_t) buffer.size());
  if (quantity == 0)
    {
      NS_LOG_INFO("DataBuffer::CreatePacket -> No data ready for sending !");
      return 0;
    }
  // Copy from front of Buffer to a new uint8_t array pointer
  uint8_t *ptrBuffer = new uint8_t[quantity];
  for (uint32_t i = 0; i < quantity; i++)
    {
      ptrBuffer[i] = buffer.front();
      buffer.pop();
    }
  // Create packet from a data pointer and its size
  Ptr<Packet> pkt = new Packet(ptrBuffer, quantity);
  delete[] ptrBuffer;

  NS_LOG_INFO("DataBuffer::CreatePacket -> freeSpaceSize == "<< bufMaxSize - (uint32_t) buffer.size() );
  return pkt;
}

uint32_t
DataBuffer::ReadPacket(Ptr<Packet> pkt, uint32_t dataLen)
{
  NS_LOG_FUNCTION (this << (int) (bufMaxSize - (uint32_t) buffer.size()) );

  uint32_t toWrite = std::min(dataLen, (bufMaxSize - (uint32_t) buffer.size()));

  if (buffer.empty() == true)
    {
      NS_LOG_INFO("DataBuffer::ReadPacket -> buffer is empty !");
    }
  else
    NS_LOG_INFO("DataBuffer::ReadPacket -> buffer was not empty !");

  uint8_t *ptrBuffer = new uint8_t[toWrite];
  pkt->CopyData(ptrBuffer, toWrite);

  for (uint32_t i = 0; i < toWrite; i++)
    buffer.push(ptrBuffer[i]);
  delete[] ptrBuffer;

  NS_LOG_INFO("DataBuffer::ReadPacket -> data   readed == "<< toWrite );NS_LOG_INFO("DataBuffer::ReadPacket -> freeSpaceSize == "<< bufMaxSize - (uint32_t) buffer.size() );
  return toWrite;
}

uint32_t
DataBuffer::PendingData()
{
  return ((uint32_t) buffer.size());
}

uint32_t
DataBuffer::FreeSpaceSize()
{
  return (bufMaxSize - (uint32_t) buffer.size());
}

bool
DataBuffer::Empty()
{
  return buffer.empty(); // ( freeSpaceSize == bufMaxSize );
}

bool
DataBuffer::Full()
{
  return (bufMaxSize == (uint32_t) buffer.size()); //( freeSpaceSize == 0 );
}
#endif
/*
MpTcpAddressInfo::MpTcpAddressInfo() :
    addrID(0), ipv4Addr(Ipv4Address::GetZero()), mask(Ipv4Mask::GetZero())
{
}

MpTcpAddressInfo::~MpTcpAddressInfo()
{
  addrID = 0;
  ipv4Addr = Ipv4Address::GetZero();
}
*/

} // namespace ns3
