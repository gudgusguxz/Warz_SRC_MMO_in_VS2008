//=============================================================================
// GBClient.h
// Copyright 2013 GameBlocks LLC, All Rights Reserved.
//=============================================================================

#ifndef PS_ID_H_
#define PS_ID_H_

#include <PSShared/Inc/GBStdInt.h>

#define GB_DllExport   __declspec( dllexport )

#define GB_ID_LENGTH_MAX 45 // 4/29/2013 matches DB column width

// note: if these change, search for "Note: Strings:" in header and update docs
#define GB_EVENT_NAME_LENGTH_MAX 20 // 10/18/2013 matches DB column width
#define GB_EVENT_PAIR_KEY_LENGTH_MAX 20 // 10/18/2013 matches DB column width

namespace GameBlocks {

class GBPublicIdImpl;
class GBInternalIdInterface;
class GB_DllExport GBPublicIdBase
{
public:
	GBPublicIdBase(const char * val);
	GBPublicIdBase(int32_t val);
	GBPublicIdBase(uint32_t val);
	GBPublicIdBase();
	GBPublicIdBase(const GBPublicIdBase & other);

	virtual ~GBPublicIdBase();

	operator int32_t () const;
	operator uint32_t () const;
	bool operator ==(const GBPublicIdBase & other) const;
	GBPublicIdBase & operator =(const GBPublicIdBase & other);

	// returns const char c string representation of internal data
	const char * c_str() const;

	virtual bool IsValid() const = 0;
	virtual bool IsNull() const = 0;

	const GBInternalIdInterface * GetInternalInterface() const;

protected:
	GBPublicIdBase(int64_t val);
	GBPublicIdBase(uint64_t val);

	operator int64_t () const;
	operator uint64_t () const;

	GBPublicIdImpl * m_impl;

private:
	GBInternalIdInterface * m_internalInterface;
};

class GB_DllExport GBPublicTargetId: public GBPublicIdBase
{
public:
	GBPublicTargetId(const char * val);
	GBPublicTargetId(int32_t val);
	GBPublicTargetId(uint32_t val);
	GBPublicTargetId(int64_t val);
	GBPublicTargetId(uint64_t val);
	GBPublicTargetId() {};
	GBPublicTargetId(const GBPublicTargetId & other);

	operator int64_t () const;
	operator uint64_t () const;

	virtual bool IsValid() const;
	virtual bool IsNull() const;

private:
};

class GB_DllExport GBPredefinedId
{
public:
	GBPredefinedId(const char * val);
	GBPredefinedId(int32_t val);
	GBPredefinedId();

	GBPredefinedId & operator =(const char * val);
	GBPredefinedId & operator =(const int32_t val);

	static void SetSeed(uint32_t val);

	int32_t Id() const;

private:
	void ConvertToInt(const char * val);

	int32_t m_id;
};

typedef GBPublicTargetId GBSourceProperty;
//typedef GBPublicTargetId GBPredefinedId;
typedef GBPublicTargetId GBPublicPlayerId;
typedef GBPublicTargetId GBPublicSourceId;

} // namespace GameBlockss


#endif // #define PS_ID_H_