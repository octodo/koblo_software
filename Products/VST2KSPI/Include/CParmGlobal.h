
// Forwards
class CVST2KSPIPlugIn;

class CParmGlobal : public virtual de::IParameter
{
public:
	CParmGlobal(CVST2KSPIPlugIn* pPlugIn, tint32 iID, tint32 iMinValue, tint32 iMaxValue, tint32 iValue = 0);

	virtual ~CParmGlobal();

	virtual void Destroy();

	// IParameter override
	virtual tint32 GetID() const;
	// IParameter override
	virtual tint32 GetDefaultValue() const;
	// IParameter override
	virtual tint32 GetValue() const;
	// IParameter override
	virtual void SetValue(tint32 iValueNew, tbool bCallOnChange = true);
	// IParameter override
	virtual tint32 GetMinValue() const;
	// IParameter override
	virtual tint32 GetMaxValue() const;
	// IParameter override
	virtual void OnChange();

protected:
	CVST2KSPIPlugIn* mpPlugIn;
	tint32 miID;
	tint32 miDefaultValue;
	tint32 miValue;
	tint32 miMinValue;
	tint32 miMaxValue;
};
