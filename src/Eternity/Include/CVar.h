#ifndef __CVAR_H_44431124932
#define __CVAR_H_44431124932

class CVar
{
  public:
	const char*	pName;
	const char*	pComment;
  public:
	CVar(const char* _pName, const char* _pComment = "");
virtual	void		Get(char* buf) = NULL;
virtual	char*		Put(char* buf) = NULL;
};

class CVarFloat : public CVar
{
  public:
	float		var;
  public:
	CVarFloat(const char* _pName) : CVar(_pName) {};
	CVarFloat(const char* _pName, float _var, const char* _pComment="") : CVar(_pName, _pComment), var(_var) {};
	char*		Put(char* buf) { sprintf_s(buf, 512, "%.3f", var); return buf; } // TODO: verify buffer size - caller passes char[512]
	void		Get(char* buf) { float v; if(sscanf_s(buf, "%f", &v) == 1) var = v; }
	__forceinline operator float() const { return var; }
};

class CVarInt : public CVar
{
  public:
	int		var;
  public:
	CVarInt(const char* _pName, int _var, const char* _pComment="") : CVar(_pName, _pComment), var(_var) {};
	char*		Put(char* buf) { sprintf_s(buf, 512, "%d", var); return buf; } // TODO: verify buffer size - caller passes char[512]
	void		Get(char* buf) { int v; if(sscanf_s(buf, "%d", &v) == 1) var = v; }
	__forceinline operator int() const { return var; }
};

class CVarString : public CVar
{
  public:
	//TODO: make it r3dSTLString or dynamic buffer
	char		var[512];
  public:
	CVarString(char *_pName, char* _var, char *_pComment="") : CVar(_pName, _pComment) { r3dscpy(var, _var); };
	char*		Put(char *buf) { sprintf_s(buf, 512, "%s", var); return buf; } // TODO: verify buffer size - caller passes char[512]
	void		Get(char *buf) { r3dscpy(var, buf); }	
	__forceinline operator char*() const { return (char *)&var[0]; }
};


#define CVAR_CONCAT(x,y)       x ## y 
#define CVAR_BYLINE(name,line) CVAR_CONCAT(name,line) 

#define CVAR_COMMENT(yy, zz)   static CVarInt CVAR_BYLINE(_cvar_Comment,__LINE__) ("@"##yy, 0, zz)
#define	CVAR_FLOAT(xx, yy, zz) CVarFloat xx(#xx, yy, zz)
#define	CVAR_INT(xx, yy, zz)   CVarInt   xx(#xx, yy, zz)
#define	CVAR_STRING(xx, yy, zz) CVarString xx(#xx, yy, zz)

extern	int		cvars_Write(const char* fname, const char* prefix = "");
extern	int		cvars_Read(const char* fname);

#endif	__CVAR_H_44431124932
