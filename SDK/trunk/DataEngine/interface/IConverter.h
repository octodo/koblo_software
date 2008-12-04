



class IConverter : public virtual IDestructable
{
public:
	//! Create linear converter
	static IConverter* CreateLinear(tint32 iParmMin, tint32 iParmMax);
	//! Create logarithmic converter
	static IConverter* CreateLog(tint32 iParmMin, tint32 iParmMax, tuint iStrength);
	//! Create "true" logarithmic converter
	static IConverter* CreateTrueLog(tint32 iParmMin, tint32 iParmMax);
	//! Create enum converter (continuous controls works as discrete control). iParmMin must be 0.
	static IConverter* CreateEnum(tint32 iParmMax);
	//! Create "x3" converter. iParmMin must be 0. iParmMax / (2 ^ iParmDecimals) must be >= 1
	static IConverter* CreateX3(tint32 iParmMin, tint32 iParmMax, tuint uiParmDecimals);

	virtual tint32 Parm2GUI(tint32 iParm) const = 0;
	virtual tint32 GUI2Parm(tint32 iGUI) const = 0;
	virtual tfloat64 Parm2Normalised(tint32 iParm) const = 0;
	virtual tint32 Normalised2Parm(tfloat64 fNormalised) const = 0;

	enum EType {
		TypeLinear = 0,
		TypeLog,
		TypeTrueLog,
		TypeEnum,
		TypeX3
	};

	virtual EType GetType() const = 0;

	//! Returns the strength. Only valid for log converter.
	virtual tuint GetStrength() const = 0;
};