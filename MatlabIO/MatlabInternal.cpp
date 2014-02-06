
#include "stdafx.h"

#include "MatlabInternal.h"
#include "tinyfl.h"

using namespace Matlab;

__internal::_root::_root(char* start, int len)
{
	char* cur_point = start;

	char* data_loc;
	std::uint32_t data_type_id;
	std::uint32_t data_len;

	while (cur_point < start + len)
	{
		if (*(cur_point + 2) == 0 && *(cur_point + 3) == 0)
		{
			// regular data format
			data_loc = cur_point + 8;
			data_len = *(reinterpret_cast<std::uint32_t*>(cur_point + 4));
			data_type_id = *(reinterpret_cast<std::uint32_t*>(cur_point));
		}
		else
		{
			// compressed data format
			data_loc = cur_point + 4;
			std::int16_t len_t = *(reinterpret_cast<std::uint16_t*>(cur_point + 2));
			std::int16_t typeid_t = *(reinterpret_cast<std::uint16_t*>(cur_point));
			data_len = len_t;
			data_type_id = typeid_t;
		}

		switch (data_type_id)
		{
		case DataTypes::miCOMPRESSED::MFT:
			children.push_back(new DataTypes::miCOMPRESSED(data_loc, data_len));
			break;
		case DataTypes::miDOUBLE::MFT:
			children.push_back(new DataTypes::miDOUBLE(data_loc, data_len));
			break;
		case DataTypes::miINT16::MFT:
			children.push_back(new DataTypes::miINT16(data_loc, data_len));
			break;
		case DataTypes::miINT32::MFT:
			children.push_back(new DataTypes::miINT32(data_loc, data_len));
			break;
		case DataTypes::miINT64::MFT:
			children.push_back(new DataTypes::miINT64(data_loc, data_len));
			break;
		case DataTypes::miINT8::MFT:
			children.push_back(new DataTypes::miINT8(data_loc, data_len));
			break;
		case DataTypes::miSINGLE::MFT:
			children.push_back(new DataTypes::miSINGLE(data_loc, data_len));
			break;
		case DataTypes::miUINT16::MFT:
			children.push_back(new DataTypes::miUINT16(data_loc, data_len));
			break;
		case DataTypes::miUINT32::MFT:
			children.push_back(new DataTypes::miUINT32(data_loc, data_len));
			break;
		case DataTypes::miUINT64::MFT:
			children.push_back(new DataTypes::miUINT64(data_loc, data_len));
			break;
		case DataTypes::miUINT8::MFT:
			children.push_back(new DataTypes::miUINT8(data_loc, data_len));
			break;
		case DataTypes::miUTF16::MFT:
			children.push_back(new DataTypes::miUTF16(data_loc, data_len));
			break;
		case DataTypes::miUTF32::MFT:
			children.push_back(new DataTypes::miUTF32(data_loc, data_len));
			break;
		case DataTypes::miUTF8::MFT:
			children.push_back(new DataTypes::miUTF8(data_loc, data_len));
			break;
		case DataTypes::miMATRIX::MFT:
			children.push_back(new DataTypes::miMATRIX(data_loc, data_len));
			break;
		default:
			std::cout << "UNSUPPORTED DATA TYPE IN MATLAB FILE: " << data_type_id << std::endl;
			break;
		}

		cur_point = data_loc + data_len;
	}
}

__internal::_compressed_type::_compressed_type(char* start, int len)
{
	compressed_data = start;
	compressed_data_len = len;

	void* uncomp = tinfl_decompress_mem_to_heap((void*)compressed_data,
		compressed_data_len,
		&uncompressed_data_len,
		TINFL_FLAG_PARSE_ZLIB_HEADER);

	if (uncomp == 0)
		throw std::exception("unable to deflate compressed variable in matlab file");

	uncompressed_data = reinterpret_cast<char*>(uncomp);

	// todo:
	// call to deflate()

	_root* ch = new _root(uncompressed_data, uncompressed_data_len);
	children.push_back(ch);
}

__internal::_matlab_array::_matlab_array(char* start, int len)
{
	char* ptr = start;
	ptr = read_single_primitive(ptr, &flags);
	ptr = read_single_primitive(ptr, &dimensions);
	ptr = read_single_primitive(ptr, &name);

	std::uint8_t flag_type = *(reinterpret_cast<std::uint8_t*>(flags->get() + 3));

	char* data_loc;
	std::uint32_t data_type_id;
	std::uint32_t data_len;

	while (ptr < start + len)
	{
		if (*(ptr + 2) == 0 && *(ptr + 3) == 0)
		{
			// regular data format
			data_loc = ptr + 8;
			data_len = *(reinterpret_cast<std::uint32_t*>(ptr + 4));
			data_type_id = *(reinterpret_cast<std::uint32_t*>(ptr));
		}
		else
		{
			// compressed data format
			data_loc = ptr + 4;
			std::int16_t len_t = *(reinterpret_cast<std::uint16_t*>(ptr + 2));
			std::int16_t typeid_t = *(reinterpret_cast<std::uint16_t*>(ptr));
			data_len = len_t;
			data_type_id = typeid_t;
		}

		switch (data_type_id)
		{
		case DataTypes::miCOMPRESSED::MFT:
			children.push_back(new DataTypes::miCOMPRESSED(data_loc, data_len));
			break;
		case DataTypes::miDOUBLE::MFT:
			children.push_back(new DataTypes::miDOUBLE(data_loc, data_len));
			break;
		case DataTypes::miINT16::MFT:
			children.push_back(new DataTypes::miINT16(data_loc, data_len));
			break;
		case DataTypes::miINT32::MFT:
			children.push_back(new DataTypes::miINT32(data_loc, data_len));
			break;
		case DataTypes::miINT64::MFT:
			children.push_back(new DataTypes::miINT64(data_loc, data_len));
			break;
		case DataTypes::miINT8::MFT:
			children.push_back(new DataTypes::miINT8(data_loc, data_len));
			break;
		case DataTypes::miSINGLE::MFT:
			children.push_back(new DataTypes::miSINGLE(data_loc, data_len));
			break;
		case DataTypes::miUINT16::MFT:
			children.push_back(new DataTypes::miUINT16(data_loc, data_len));
			break;
		case DataTypes::miUINT32::MFT:
			children.push_back(new DataTypes::miUINT32(data_loc, data_len));
			break;
		case DataTypes::miUINT64::MFT:
			children.push_back(new DataTypes::miUINT64(data_loc, data_len));
			break;
		case DataTypes::miUINT8::MFT:
			children.push_back(new DataTypes::miUINT8(data_loc, data_len));
			break;
		case DataTypes::miUTF16::MFT:
			children.push_back(new DataTypes::miUTF16(data_loc, data_len));
			break;
		case DataTypes::miUTF32::MFT:
			children.push_back(new DataTypes::miUTF32(data_loc, data_len));
			break;
		case DataTypes::miUTF8::MFT:
			children.push_back(new DataTypes::miUTF8(data_loc, data_len));
			break;
		case DataTypes::miMATRIX::MFT:
			children.push_back(new DataTypes::miMATRIX(data_loc, data_len));
			break;
		case 0x0:
			break;
		default:
			// std::cout << "UNSUPPORTED DATA TYPE IN MATLAB FILE: " << data_type_id << std::endl;
			break;
		}

		ptr = data_loc + data_len;
	}

	
}