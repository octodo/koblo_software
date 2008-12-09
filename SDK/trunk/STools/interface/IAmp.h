
class IAmp
{
	public:
		static IAmp* Create();
		virtual void Destroy() = 0;
		
		// Amp
		virtual void Process(tfloat fClip, tfloat32* pfLeft,tfloat32* pfRight, tint lC) = 0;
		virtual void Process32(tfloat fClip, tfloat32* pfLeft,tfloat32* pfRight) = 0;
		
		
};



