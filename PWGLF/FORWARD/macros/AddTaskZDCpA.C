AliAnalysisTaskSE* AddTaskZDCpA(Bool_t  applyPS = kTRUE,
				  Float_t centrlowlim = 0.,
                                  Float_t centruplim = 100.,
                                  TString centrest = "V0M",
				  TString outfname = "ZDCpA",
				  Bool_t  isMC = kFALSE)
{
  
  // Get the pointer to the existing analysis manager via the static access method.
  //==============================================================================
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if(!mgr){
    ::Error("AddTaskZDCpA", "No analysis manager to connect to.");
    return NULL;
  }  

  AliVEventHandler *inputHandler = mgr->GetInputEventHandler();
  
  // Check the analysis type using the event handlers connected to the analysis manager.
  //==============================================================================
  if(!inputHandler){
    ::Error("AddTaskZDCpA", "This task requires an input event handler");
    return NULL;
  }
  TString inputDataType = inputHandler->GetDataType(); // can be "ESD" or "AOD"
  
   // Configure analysis
   //===========================================================================
   AliAnalysisTaskZDCpA* task = new AliAnalysisTaskZDCpA("taskZDCpA");

   if(inputDataType.CompareTo("ESD")==0){
      task->SetInput(1);
      //printf("  AliAnalysisTaskZDCpA initialized for ESD analysis\n");
      //
      // apply physics selection
      if(applyPS) task->SelectCollisionCandidates();
   }
   else if(inputDataType.CompareTo("AOD")==0){
      task->SetInput(2);
      //printf("  AliAnalysisTaskZDCpA initialized for AOD analysis\n");
   }
   task->SetCentralityRange(centrlowlim, centruplim);
   task->SetCentralityEstimator(centrest);
   
   if(isMC==kTRUE) task->SetMCInput();

   mgr->AddTask(task);

   TString outputFileName = AliAnalysisManager::GetCommonFileName();
   
   AliAnalysisDataContainer *coutput  = mgr->CreateContainer(outfname.Data(), 
   					TList::Class(),
					AliAnalysisManager::kOutputContainer, 	
					Form("%s:ZDChistos", mgr->GetCommonFileName()));

   mgr->ConnectInput  (task, 0, mgr->GetCommonInputContainer());
   mgr->ConnectOutput (task, 1, coutput);

   return task;   
}


