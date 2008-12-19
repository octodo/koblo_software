class CDownloader : public virtual IDownloader
{
public:
	CDownloader();
	virtual ~CDownloader();
	
	//! IDestructable override
	virtual void Destroy();
};