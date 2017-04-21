class CBaseServer
{
public:
	bool Init();
	
	virtual bool OnInit()=0;

	void Run();
	
	virtual bool OnRun()=0;

	void Release();

	void daemonize();
};
