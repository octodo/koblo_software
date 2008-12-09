
class IClipper
{
	public:
		static IClipper* Create();
		virtual void Destroy() = 0;
		
		// Soft distortion
		virtual void Process(tfloat fClip, tfloat32* pfLeft,tfloat32* pfRight, tint lC) = 0;
		
		
};



