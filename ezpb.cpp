/*!
\file ezpb.cpp
test ec_protobuf with C++ 11

\author	jiangyong
\email  kipway@outlook.com
\date   2020.8.11
*/

#ifdef _WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#	define _CRT_SECURE_NO_WARNINGS 1
#endif
#	pragma warning (disable : 4995)
#	pragma warning (disable : 4996)
#	pragma warning (disable : 4200)
#	ifndef _WIN32_WINNT
#		define _WIN32_WINNT 0x0600	//0x600 = vista/2008; 0x601=win7 ;  0x0501=windows xp ;  0x0502=windows 2003
#	endif
#else
#	include <termios.h>
#	include <unistd.h>
#endif
#include <cstdio>
#include <cstdint>
#include "ec_protobuf.h"
#include <vector>

using bytes = std::basic_string<uint8_t>;

/*
\biref 消息报文定义
syntax = "proto3";
message msg_pkg{
	uint32 order = 1; //命令,必有
	uint32 seqno = 2; //request/response配对序列号，可选，如果请求时带有此字段，则应答时需要原样返回。
	string srcid = 3; //源id
	string destid =4; //目的id,
	// order,seqno,srcid,destid 为消息头

	bytes body = 5;   //消息体，可选，根据前面的order不同解析不同。嵌入message测试
}
*/

template<typename _Tp>
class pb_msgpkg : public ec::cls_protoc3<> //消息报文模板
{
public:
	pb_msgpkg() {
		reset();
	}
public:
	typedef _Tp		value_type;
	enum {
		id_od = 1,
		id_seqno,
		id_src,
		id_dest,
		id_body
	};

	uint32_t	_order;
	uint32_t	_seqno;
	std::string _srcid;
	std::string _destid;
	value_type	_body;  //消息体,嵌入message测试

public:
	virtual void reset()
	{
		_order = 0;
		_seqno = 0;
		_srcid.clear();
		_destid.clear();
		_body.reset();
	}
	void tst_prt()
	{
		std::printf("order = %u\n", _order);
		std::printf("seqno = %u\n", _seqno);
		std::printf("srcid = %s\n", _srcid.c_str());
		std::printf("destid = %s\n", _destid.c_str());
		std::printf("cls:\n");
		_body.tst_prt();
	}
protected:
	virtual size_t size_content()
	{
		return size_var(id_od, _order) + size_var(id_seqno, _seqno) + _body.size(id_body);
	}
	virtual bool out_content(bytes* pout)
	{
		return out_var(pout, id_od, _order) && out_var(pout, id_seqno, _seqno)
			&& out_str(pout, id_src, _srcid.data(), _srcid.size()) && out_str(pout, id_dest, _destid.data(), _destid.size())
			&& _body.serialize(id_body, pout);
	}
	virtual bool on_cls(uint32_t field_number, const void* pdata, size_t sizedata)
	{
		switch (field_number)
		{
		case id_src:
			_srcid.assign((const char*)pdata, sizedata);
			break;
		case id_dest:
			_destid.assign((const char*)pdata, sizedata);
			break;
		case id_body:
			if (!_body.parse(pdata, sizedata))
				return false;
			break;
		}
		return true;
	}
	virtual bool on_var(uint32_t field_number, uint64_t val)
	{
		if (id_od == field_number)
			_order = static_cast<uint32_t>(val);
		else if (id_seqno == field_number)
			_seqno = static_cast<uint32_t>(val);
		return true;
	}
};

/**

登录消息体

syntax = "proto3";
message body_login{ //登录和返回数据
	string name = 1; //登录名
	string pswd =2;  //登录密码
	sint32 retcode = 14; //zigzag测试，返回代码，0表示成功，返回时有
	string retmsg = 15; //返回信息，可选，retcode != 0时，填写错误信息。
	repeated int32 vints = 16 [packed = true]; //测试紧凑int数组
	repeated float vfloats = 17 [packed = true]; //测试紧float凑数组
	repeated double vdoubles = 18 [packed = true]; //测试紧凑double数组
	float   f32 = 19; //测试单个float , fix32编码
	double  f64 = 20; //测试单个double , fix64编码
}
*/

class body_login : public ec::cls_protoc3<> //登录消息
{
public:
	body_login()
	{
	}
	enum {
		id_name = 1,
		id_pswd,
		id_retcode = 14,
		id_retmsg,
		id_vints,
		id_vfloats,
		id_vdoubles,
		id_float,
		id_double
	};
public:
	std::string _name;
	std::string _pswd;
	int32_t _retcode;
	std::string _retmsg;
	std::vector<int32_t> _vints;
	std::vector<float> _vfloats;
	std::vector<double> _vdoubles;
	float _f32;
	double _f64;
public:
	virtual void reset()
	{
		_name.clear();
		_pswd.clear();
		_retcode = 0u;
		_retmsg.clear();
		_vints.clear();
		_vfloats.clear();
		_vdoubles.clear();
		_f32 = 0;
		_f64 = 0;
	}

	void tst_prt()
	{
		std::printf("name   : %s\n", _name.c_str());
		std::printf("pswd   : %s\n", _pswd.c_str());
		std::printf("retcode: %d\n", _retcode);
		std::printf("retmsg : %s\n", _retmsg.c_str());
		std::printf("vints:\n");
		for (auto &i : _vints)
			std::printf("%d ", i);
		std::printf("\n");

		for (auto &i : _vfloats)
			std::printf("%G ", i);
		std::printf("\n");

		for (auto &i : _vdoubles)
			std::printf("%G ", i);
		std::printf("\n");

		std::printf("f32= %G\n", _f32);
		std::printf("f64= %G\n", _f64);
	}
protected:
	virtual size_t size_content()
	{		
		return size_str(id_name, _name.data(), _name.size())
			+ size_str(id_pswd, _pswd.data(), _pswd.size())
			+ size_var(id_retcode, _retcode, true) // user zigzag encode
			+ size_str(id_retmsg, _retmsg.data(), _retmsg.size())
			+ size_varpacket(id_vints, _vints.data(), _vints.size())
			+ size_fix32packet(id_vfloats, _vfloats.data(), _vfloats.size())
			+ size_fix64packet(id_vdoubles, _vdoubles.data(), _vdoubles.size())
			+ size_fixed(id_float, _f32)
			+ size_fixed(id_double, _f64)
			;
	}
	virtual bool out_content(bytes* pout)
	{
		return out_str(pout, id_name, _name.data(), _name.size()) && out_str(pout, id_pswd, _pswd.data(), _pswd.size())
			&& out_var(pout, id_retcode, _retcode, true) // user zigzag encode
			&& out_str(pout, id_retmsg, _retmsg.data(), _retmsg.size())
			&& out_varpacket(id_vints, _vints.data(), _vints.size(), pout)
			&& out_fix32packet(id_vfloats, _vfloats.data(), _vfloats.size(), pout)
			&& out_fix64packet(id_vdoubles, _vdoubles.data(), _vdoubles.size(), pout)
			&& out_fixed32(pout, id_float, _f32)
			&& out_fixed64(pout, id_double, _f64)
			;
	}
	virtual bool on_cls(uint32_t field_number, const void* pdata, size_t sizedata)
	{
		switch (field_number)
		{
		case id_name:
			_name.assign((const char*)pdata, sizedata);
			break;
		case id_pswd:
			_pswd.assign((const char*)pdata, sizedata);
			break;
		case id_retmsg:
			_retmsg.assign((const char*)pdata, sizedata);
			break;
		case id_vints: // packed = true
			_vints.clear();
			{
				const uint8_t* pd = (const uint8_t*)pdata;
				int len = (int)sizedata;
				int32_t nv;
				while (len > 0 && p_varpacket(pd, len, nv))
					_vints.push_back(nv);
			}
			break;
		case id_vfloats: // packed = true
			_vfloats.clear();
			{
				const uint8_t* pd = (const uint8_t*)pdata;
				int len = (int)sizedata;
				float v;
				while (len > 0 && get_fixed(pd, len, v))
					_vfloats.push_back(v);
			}
			break;
		case id_vdoubles: // packed = true
			_vdoubles.clear();
			{
				const uint8_t* pd = (const uint8_t*)pdata;
				int len = (int)sizedata;
				double v;
				while (len > 0 && get_fixed(pd, len, v))
					_vdoubles.push_back(v);
			}
			break;
		}
		return true;
	}
	virtual bool on_var(uint32_t field_number, uint64_t val)
	{
		switch (field_number)
		{
		case id_retcode:
			_retcode = static_cast<int>(ec::base_protobuf::t_zigzag<uint64_t>().decode(val));
			break;
		case id_vints: // 兼容packed = false
			_vints.push_back((int32_t)val);
			break;
		}
		return true;
	}
	virtual bool on_fix32(uint32_t field_number, const void* pval)
	{
		if (id_vfloats == field_number) // 兼容packed = false
			_vfloats.push_back(*(float*)pval);

		else if (id_float == field_number)
			_f32 = *(float*)pval;
		return true;
	}
	virtual bool on_fix64(uint32_t field_number, const void* pval)
	{
		if (id_vdoubles == field_number) // 兼容packed = false
			_vdoubles.push_back(*(double*)pval);
		else if (id_double == field_number)
			_f64 = *(double*)pval;
		return true;
	}
};

using scmsg_login = pb_msgpkg<body_login>; //完整的登录报文

int main()
{
	scmsg_login pkgin, pkgout;
	bytes frmbuf;
	frmbuf.reserve(512);

	pkgin._order = 10;
	pkgin._seqno = 100;
	pkgin._srcid = "srcid1";
	pkgin._destid = "destid1";
	pkgin._body._name = "user1";
	pkgin._body._pswd = "pswd1";
	pkgin._body._retcode = -5;
	pkgin._body._retmsg = "login ret message";
	for (auto i = 100; i < 110; i++)
		pkgin._body._vints.push_back(i);

	for (auto i = 10; i < 21; i++)
		pkgin._body._vfloats.push_back((float)(i + 0.1));

	for (auto i = 20; i < 31; i++)
		pkgin._body._vdoubles.push_back((double)(i + 0.2));
	pkgin._body._f32 = 12.5;
	pkgin._body._f64 = 125.5;
	std::printf("print pkgin:\n");
	pkgin.tst_prt();

	if (!pkgin.serialize(&frmbuf)) {
		std::printf("pkgin.serialize failed\n");
		return 1;
	}
	std::printf("pkgin.serialize success, size = %zu\n", frmbuf.size());

	if (!pkgout.parse(frmbuf.data(), frmbuf.size())) {
		std::printf("pkgout.parse failed!\n");
		return 2;
	}

	std::printf("\nprint pkgout:\n");
	pkgout.tst_prt();
}