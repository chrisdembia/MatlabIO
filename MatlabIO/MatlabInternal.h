#ifndef MATLAB_INTERNAL_H
#define MATLAB_INTERNAL_H

#include <iostream>
#include <cstdint>
#include <list>
#include <fstream>
#include <sstream>

namespace Matlab
{
	namespace __internal
	{
		struct _data_type_base
		{
			virtual const std::list<_data_type_base*>& nodes() const
			{
				return children;
			}
			virtual std::ostream& operator>>(std::ostream& src) = 0;
			virtual ~_data_type_base() { }
			virtual const std::string to_string(bool cast = false) const
			{
				return "";
			}
			virtual const std::int32_t mft() const = 0;
			static const std::int32_t MFT = 0x0;
		protected:
			std::list<_data_type_base*> children;
		};

		template<class T, std::int32_t _MFT>
		struct _basic_type : public _data_type_base
		{
			_basic_type(char* start, int len)
			{
				val = reinterpret_cast<T*>(start);
				length = len / sizeof(T);
			}
			std::ostream& operator>>(std::ostream& src)
			{
				src << MFT;
				src << length;
				for (std::uint32_t i = 0; i < length; ++i)
					src << val[i];
				return src;
			}
			T* get() const
			{
				return val;
			}
			const std::uint32_t len() const
			{
				return length;
			}
			const std::int32_t mft() const
			{
				return _MFT;
			}
			const std::string to_string(bool cast = false) const
			{
				std::stringstream ss;
				for (std::uint32_t i = 0; i < length; ++i)
				if (cast)
					ss << std::to_string(val[i]) << " ";
				else
					ss << val[i];
				return ss.str();
			}
			static const std::int32_t MFT = _MFT;
		private:
			T* val;
			std::uint32_t length;
		};

		struct _root : public _data_type_base
		{
			const std::int32_t mft() const
			{
				return 0x00;
			}
			static const std::int32_t MFT = 0x00;

			_root(char* start, int len);

			std::ostream& operator>>(std::ostream& src)
			{
				// todo...
				return src;
			}

		};

		struct _compressed_type : public _data_type_base
		{
			static const std::int32_t MFT = 0x0F;
			_compressed_type(char* start, int len);
			~_compressed_type()
			{
				delete[] uncompressed_data;
			}
			std::ostream& operator>>(std::ostream& src)
			{
				// todo...
				return src;
			}
			const std::int32_t mft() const
			{
				return 0x0F;
			}
		private:
			std::uint32_t compressed_data_len;
			std::uint32_t uncompressed_data_len;
			char* compressed_data;
			char* uncompressed_data;
		};
	}

	namespace DataTypes
	{
		typedef __internal::_basic_type<std::int8_t, 0x01> miINT8;
		typedef __internal::_basic_type<std::uint8_t, 0x02> miUINT8;
		typedef __internal::_basic_type<std::int16_t, 0x03> miINT16;
		typedef __internal::_basic_type<std::uint16_t, 0x04> miUINT16;
		typedef __internal::_basic_type<std::int32_t, 0x05> miINT32;
		typedef __internal::_basic_type<std::uint32_t, 0x06> miUINT32;
		typedef __internal::_basic_type<float, 0x07> miSINGLE;
		typedef __internal::_basic_type<double, 0x09> miDOUBLE;
		typedef __internal::_basic_type<std::int64_t, 0x0C> miINT64;
		typedef __internal::_basic_type<std::uint64_t, 0x0D> miUINT64;
		typedef __internal::_compressed_type miCOMPRESSED;
		typedef __internal::_basic_type<std::int8_t, 0x10> miUTF8;
		typedef __internal::_basic_type<std::int16_t, 0x11> miUTF16;
		typedef __internal::_basic_type<std::int32_t, 0x12> miUTF32;
	}
	namespace ArrayTypes
	{
		typedef __internal::_basic_type<char, 0x04> mxCHAR_CLASS;
		typedef __internal::_basic_type<double, 0x06> mxDOUBLE_CLASS;
		typedef __internal::_basic_type<float, 0x07> mxSINGLE_CLASS;
		typedef __internal::_basic_type<std::int8_t, 0x08> mxINT8_CLASS;
		typedef __internal::_basic_type<std::uint8_t, 0x09> mxUINT8_CLASS;
		typedef __internal::_basic_type<std::int16_t, 0x0A> mxINT16_CLASS;
		typedef __internal::_basic_type<std::uint16_t, 0x0B> mxUINT16_CLASS;
		typedef __internal::_basic_type<std::int32_t, 0x0C> mxINT32_CLASS;
		typedef __internal::_basic_type<std::uint32_t, 0x0D> mxUINT32_CLASS;
		typedef __internal::_basic_type<std::int64_t, 0x0E> mxINT64_CLASS;
		typedef __internal::_basic_type<std::uint64_t, 0x0F> mxUINT64_CLASS;
	}

	namespace __internal
	{
		struct _matlab_array : public _data_type_base
		{
			static const std::int32_t MFT = 0x0E;
			DataTypes::miUINT32* flags;
			DataTypes::miINT32* dimensions;
			DataTypes::miINT8* name;

			_matlab_array(char* start, int len);

			std::ostream& operator>>(std::ostream& src)
			{
				// todo...
				return src;
			}
			~_matlab_array()
			{
				delete flags, dimensions, name;
			}
			const std::int32_t mft() const
			{
				return 0x0E;
			}

			const std::string to_string(bool cast = false) const
			{
				std::stringstream ss;

				ss << "Name: " << name->to_string() << std::endl;
				ss << "--Dims:" << dimensions->to_string() << std::endl;

				int j = 0;

				for (auto child : children)
					ss << "--Child " << j++ << ": " << child->to_string(true) << std::endl;
				
				return ss.str();

			}
		private:
			template<class T>
			char* read_single_primitive(char* src, T** dest)
			{
				char* data_loc;
				std::uint32_t data_type_id;
				std::uint32_t data_len;
				if (*(src + 2) == 0 && *(src + 3) == 0)
				{
					// regular data format
					data_loc = src + 8;
					data_len = *(reinterpret_cast<std::uint32_t*>(src + 4));
					data_type_id = *(reinterpret_cast<std::uint32_t*>(src));
				}
				else
				{
					// compressed data format
					data_loc = src + 4;
					std::int16_t len_t = *(reinterpret_cast<std::uint16_t*>(src));
					std::int16_t typeid_t = *(reinterpret_cast<std::uint16_t*>(src + 2));
					data_len = len_t;
					data_type_id = typeid_t;
				}
				*dest = new T(data_loc, data_len);

				std::size_t padding_bts = 0;
				if ((data_loc + data_len - src) % 8 != 0)
					padding_bts = 8 - ((data_loc + data_len - src) % 8);

				return data_loc + data_len + padding_bts;
			}
		};


		struct _file
		{
			const std::list<_matlab_array*>& matricies() const
			{
				return _matricies;
			}
			const std::string& description() const
			{
				return _desc;
			}
			const std::list<_data_type_base*>& nodes() const
			{
				return root->nodes();
			}
			_file(std::string file_name)
			{
				std::ifstream file(file_name, std::ifstream::binary);

				file.seekg(0, file.end);
				std::streamoff length = file.tellg();

				file.seekg(0, file.beg);

				_tot_len = static_cast<std::size_t>(length);

				_data = new char[_tot_len];
				file.read(_data, _tot_len);

				std::stringstream header_stream;
				for (int i = 0; i < 116; ++i)
					header_stream << _data[i];
				_desc = header_stream.str();

				_subs_offset = _data + 116;
				_flag_version = _subs_offset + 8;
				_end_indicator = _flag_version + 2;

				// todo: endian handling...

				root = new _root(_end_indicator + 2, _tot_len - 128);

				_rec_find_mat(root);
			}

			~_file()
			{
				delete[] _data;
			}
		private:
			void _rec_find_mat(_data_type_base* cur)
			{
				if (cur->mft() == __internal::_matlab_array::MFT)
					_matricies.push_back(dynamic_cast<_matlab_array*>(cur));

				for (_data_type_base* child : cur->nodes())
					_rec_find_mat(child);
			}

			_root* root;
			std::string _desc;
			char* _subs_offset;
			char* _flag_version;
			char* _end_indicator;
			char* _data;
			bool _new;
			std::size_t _tot_len;
			std::list<_matlab_array*> _matricies;
		};

	}

	namespace DataTypes
	{
		typedef __internal::_matlab_array miMATRIX;
	}

	/*
		Classes exposed to users:
	*/

	typedef __internal::_file File;
	typedef __internal::_data_type_base Element;
	typedef __internal::_root ElementContainer;
	typedef __internal::_compressed_type CompressedNode;
	typedef __internal::_matlab_array Matrix;
}

#endif