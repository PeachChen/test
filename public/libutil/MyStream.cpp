#include "MyStream.h"

Stream::Stream(uint8* pBuffer, size_t size) :m_Buffer(pBuffer), m_Pointer(pBuffer), m_Size(size)
{
	m_IsNew = false;
	assert(pBuffer);
}

Stream::Stream(size_t size)
{
	m_IsNew = true;
	m_Size = size;
	m_Buffer = new uint8[m_Size];
	m_Pointer = m_Buffer;
}

void Stream::ReleaseBuffer()
{
	if (m_IsNew)
	{
		if (m_Buffer!=NULL)
			delete[] m_Buffer;
		m_Buffer = NULL;
		m_Pointer = m_Buffer;
		m_Size = 0;
	}
}

void Stream::SetPointer(size_t size)
{
	if (size > m_Size)
		return;
	m_Pointer += size;
}