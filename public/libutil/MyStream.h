//�����ݲ���
#pragma once
#include "DefineHeader.h"

enum
{
	STREAM_MAX_STRING_LEN=1024*1024*10,
	STREAM_MAX_VECTOR_LEN=1024*1024*10,
	STREAM_MAX_MAP_LEN=1024*1024*10,
};

enum EStreamError
{
	EStreamError_Error,
	EStreamError_OK,
	EStreamError_BufferFull,
	EStreamError_BadArg,
};

//�����ݽӿ�
class BaseStream
{
public:
	uint8 m_StreamError; //����

	BaseStream() :m_StreamError(EStreamError_OK){};
	virtual ~BaseStream(){};
		
	EStreamError GetError() const { return (EStreamError)m_StreamError; }
	bool IsOK() const { return (m_StreamError == EStreamError_OK); }
	
	//���л�
	virtual BaseStream& BytesSerialize(const void* data, size_t size) = 0;
	//�����л�
	virtual BaseStream& BytesDeserialize(void* data, size_t size) = 0;
	//������ʼ��ַ
	//virtual uint8* GetBuffer() = 0;
	//ƫ��
	virtual size_t GetOffset() = 0;
	//�ܴ�С
	virtual size_t GetSize() = 0;
	//���д�С
	virtual size_t GetSpace() { return GetSize() - GetOffset(); }

#define BASESTREAM_SERIALIZE(valueType) \
	BaseStream& operator << (valueType value) \
	{\
		return BytesSerialize(&value, sizeof(valueType)); \
	}\
	BaseStream& operator >> (valueType& value) \
	{\
		return BytesDeserialize(&value, sizeof(valueType));\
	}

	BASESTREAM_SERIALIZE(int);
	BASESTREAM_SERIALIZE(uint);
	BASESTREAM_SERIALIZE(int8);
	BASESTREAM_SERIALIZE(uint8);
	BASESTREAM_SERIALIZE(int16);
	BASESTREAM_SERIALIZE(uint16);
	BASESTREAM_SERIALIZE(int64);
	BASESTREAM_SERIALIZE(uint64);
	BASESTREAM_SERIALIZE(float);
	BASESTREAM_SERIALIZE(double);

	BaseStream& operator << (bool value)
	{
		uint8 i = (value ? 1 : 0);
		return BytesDeserialize(&i, sizeof(uint8));
	}

	BaseStream& operator >> (bool& value)
	{
		uint8 i = 0;
		BytesDeserialize(&i, sizeof(uint8));
		value = (i == 1);
		return *this;
	}

	BaseStream& operator << (const char* str)
	{
		if (str == NULL)
		{
			m_StreamError = EStreamError_BadArg;
			return *this;
		}
		uint size = (uint)strlen(str);
		if (size > STREAM_MAX_MAP_LEN)
		{
			m_StreamError = EStreamError_BadArg;
			return *this;
		}
		BytesSerialize(&size, sizeof(uint));
		return BytesSerialize(str, size);
	}

	BaseStream& operator << (const string& str)
	{
		uint size = (uint)str.length();//����64λ��32λ��һ����
		if (size > STREAM_MAX_STRING_LEN)
		{
			m_StreamError = EStreamError_BadArg;
			Assert(false);
			return *this;
		}
		BytesSerialize(&size, sizeof(uint));
		return BytesSerialize(str.c_str(), size);
	}

	BaseStream& operator >> (string& str)
	{
		uint size = 0;
		BytesDeserialize(&size, sizeof(uint));
		str.clear();
		if (size > 0)
		{
			if (size > STREAM_MAX_STRING_LEN)
			{
				m_StreamError = EStreamError_BadArg;
				Assert(false);
				return *this;
			}
			str.insert(0, size, 0);
			return BytesDeserialize(&(str[0]), size);
		}
		return *this;		
	}

	//for Lua script system
	int8 ReadInt8()
	{
		int8 v = 0;
		BytesDeserialize(&v, sizeof(int8));
		return v;
	}

	void WriteInt8(int8 v)
	{
		BytesSerialize(&v, sizeof(int8));
	}

	uint8 ReadUint8()
	{
		uint8 v = 0;
		BytesDeserialize(&v, sizeof(uint8));
		return v;
	}

	void WriteUint8(uint8 v)
	{
		BytesSerialize(&v, sizeof(uint8));
	}

	int ReadInt()
	{
		int v = 0;
		BytesDeserialize(&v, sizeof(int));
		return v;
	}

	void WriteInt(int v)
	{
		BytesSerialize(&v, sizeof(int));
	}

	std::string ReadStr()
	{
		uint size = 0;
		BytesDeserialize(&size, sizeof(uint));
		std::string str;
		str.clear();
		if (size > 0)
		{
			str.insert(0, size, 0);
			BytesDeserialize(&str[0], size);
		}
		return str;
	}
};

//ȫ��ģ�� ��ͨ�������͵����л�
//��Ҫ��ͨ���������Լ�ʵ��Serialize��Deserialize
template<typename T>
inline BaseStream& operator<<(BaseStream& os, const T& data)
{
	data.Serialize(os);
	return os;
}

template<typename T>
inline BaseStream& operator>>(BaseStream& is, T& data)
{
	data.Deserialize(is);
	return is;
}

//vector ���л�
template<typename T>
inline BaseStream& operator<<(BaseStream& os, const std::vector<T>& data)
{
	uint size = (uint)data.size();
	if (size > STREAM_MAX_VECTOR_LEN)
	{
		os.m_StreamError = EStreamError_BadArg;
		return os;
	}
	os << size;
	for (uint i = 0; i < size; ++i)
	{
		os << data[i];
	}
	return os;
}

template<typename T>
inline BaseStream& operator>>(BaseStream& is, std::vector<T>& data)
{
	uint size =0;
	is >> size;
	if (size > STREAM_MAX_VECTOR_LEN)
	{
		is.m_StreamError = EStreamError_BadArg;
		return is;
	}
	data.resize((size_t)size);
	for (uint i = 0; i < size; ++i)
	{
		is >> data[i];
	}
	return is;
}


//�̶������������
class Stream :public  BaseStream
{
protected:
	bool m_IsNew;
	uint8* m_Buffer; //��ʼ��ַ
	uint8* m_Pointer;//��ǰ��ַ
	size_t m_Size; //�ܴ�С
public:
	//�ⲿ���ݳ�ʼ��
	Stream(uint8* pBuffer, size_t size);
	//�ڲ�����̶�size
	Stream(size_t size);
	virtual ~Stream(){ ReleaseBuffer(); }

	bool IsOk() const { return (m_StreamError == EStreamError_OK); }
	//�ͷ��ڴ�
	void ReleaseBuffer();
	//ƫ��
	virtual size_t GetOffset() { return (size_t)(m_Pointer - m_Buffer); }
	//�ܴ�С
	virtual size_t GetSize() { return m_Size; }
	//ָ��ӿ�
	uint8* GetBuffer() { return m_Buffer; }
	uint8* GetOffsetPointer(){ return m_Pointer; }
	void ResetPointer(){ m_Pointer = m_Buffer; }
	void SetPointer(size_t size);
	
	//д����
	bool SetData(size_t pos, const void* data, size_t size)
	{
		if (pos + size > m_Size)
		{
			assert(false);
			m_StreamError = EStreamError_BufferFull;
			return false;
		}
		memcpy(&(m_Buffer[pos]), data, size);
		return true;
	}

	//ƫ����ǰ
	bool Ignore(size_t size)
	{
		if (GetSpace() < size)
		{
			assert(false);
			m_StreamError = EStreamError_BufferFull;
			return false;
		}
		m_Pointer += size;
		return true;
	}

	//���л�
	virtual BaseStream& BytesSerialize(const void* data, size_t size)
	{
		if (GetSpace() < size)
		{
			assert(false);
			m_StreamError = EStreamError_BufferFull;
			return *this;
		}
		memcpy(m_Pointer, data, size);
		m_Pointer += size;
		return *this;
	}
	//�����л�
	virtual BaseStream& BytesDeserialize(void* data, size_t size)
	{
		if (GetSpace() < size)
		{
			m_StreamError = EStreamError_BufferFull;
			return *this;
		}
		memcpy(data, m_Pointer, size);
		m_Pointer += size;
		return *this;
	}


	//���л��Լ�
	virtual void Serialize(BaseStream& streamTo)
	{
		if (this == &streamTo)
		{
			return;
		}
		streamTo.BytesSerialize(GetBuffer(), GetOffset());
	}
	virtual void Deserialize(BaseStream& streamFrom)
	{
		if (this == &streamFrom)
		{
			return;
		}
		size_t len = streamFrom.GetOffset();
		streamFrom.BytesDeserialize(GetBuffer(), len);
		SetPointer(len);
	}
};

//�̶���С��������
template<uint BufferSize>
class BufferStream :public Stream
{
protected:
	uint8 m_MemBuffer[BufferSize];
public:
	BufferStream() :Stream(m_MemBuffer, BufferSize)
	{
		memset(m_MemBuffer, 0, sizeof(m_MemBuffer));
	}
};

//��̬��С��������
class StringStream :public BaseStream
{
protected:
	vector<int8> m_Buffer;
	size_t m_Offset;
public:
	StringStream() { m_Offset = 0; }
	virtual ~StringStream() {}

	virtual size_t GetOffset(){ return m_Offset; }
	virtual size_t GetSize() { return m_Buffer.size(); }

	vector<int8>& GetBuffer() { return m_Buffer; }

	const vector<int8>& GetBuffer() const { return m_Buffer; }
	void ResetOffset() { m_Offset = 0; }
	void Clean()
	{
		ResetOffset();
		m_Buffer.clear();
	}
	//���������м���������
	bool SetData(size_t pos, const void* data, size_t size)
	{
		m_Buffer.resize(m_Offset + size);
		memcpy(&(m_Buffer[pos]), data, size);
		return true;
	}

	void swap(StringStream& rs)
	{
		m_Buffer.swap(rs.m_Buffer);
		std::swap(m_Offset, rs.m_Offset);
	}

	//���л�
	virtual BaseStream& BytesSerialize(const void* data, size_t size)
	{
		if (data == NULL)
		{
			m_StreamError = EStreamError_BadArg;
			return *this;
		}
		if (size)
		{
			m_Buffer.resize(m_Offset + size);
			memcpy(&(m_Buffer[m_Offset]), data, size);
			m_Offset += size;
		}
		return *this;
	}
	virtual BaseStream& BytesDeserialize(void* data, size_t size)
	{
		if (data == NULL)
		{
			m_StreamError = EStreamError_BadArg;
			return *this;
		}
		if (m_Offset + size > m_Buffer.size())
		{
			m_StreamError = EStreamError_BufferFull;
			return *this;
		}
		memcpy(data, &(m_Buffer[m_Offset]), size);
		m_Offset += size;
		return *this;
	}

	//���л��Լ�
	virtual void Serialize(BaseStream& streamTo)
	{
		if (this == &streamTo)
		{
			assert(false);
			return;
		}
		streamTo << (int)GetOffset();
		streamTo.BytesSerialize(&GetBuffer()[0], GetOffset());
	}

	virtual void Deserialize(BaseStream& streamFrom)
	{
		if (this == &streamFrom)
		{
			m_StreamError = EStreamError_BadArg;
			return;
		}
		int dataSize = 0;
		streamFrom >> dataSize;
		m_Buffer.resize(dataSize);
		streamFrom.BytesDeserialize(&GetBuffer()[0], dataSize);
	}
};
