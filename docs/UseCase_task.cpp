
void mult(float const* a, float const*b, size_t NumData, float* dst)
{
	CalcDevice pc;
	
	pc.ClearData();
	pc.SetSpacePerTask(sizeof(float)*3);
	
	pc.WritePerTask(
		/*Target data*/a, 
		/*Number of tasks*/NumData, 
		/*Ofst from task space*/0,
		/*Size per write*/sizeof(float)
	);
	pc.WritePerTask(
		/*Target data*/b, 
		/*Number of tasks*/NumData, 
		/*Ofst from task space*/4,
		/*Size per write*/sizeof(float)
	);
	
	pc.ClearTaskQueue();
	pc.QueueTaskLoadWord( 
		// This operation will execute
		// 	LDR DST, [r15 + OFST]
		// per task
		/*Target Register*/ r1,
		/*Data Ofst*/0
	);
	pc.QueueTaskLoadWord(
		/*Target Register*/ r2,
		/*Data Ofst*/4
	);
	pc.QueueTaskFMul(
		/*Target Register*/ r0,
		/*OPR A, B*/r1,
		            r2
	);
	pc.QueueTaskStoreWord(
		/*Target Register*/ r0,
		/*Data Ofst*/8
	);
	
	pc.ExecuteTaskQueue();
	pc.Flush();
	pc.ReadPerTask(
		/*Destination*/dst, 
		/*Number of tasks*/NumData, 
		/*Ofst from task space*/8,
		/*Size per read*/sizeof(float)
	);
}