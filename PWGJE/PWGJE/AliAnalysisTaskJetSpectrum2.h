#ifndef ALIANALYSISTASKJETSPECTRUM2_H
#define ALIANALYSISTASKJETSPECTRUM2_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// **************************************
// Task used for the correction of determiantion of reconstructed jet spectra
// Compares input (gen) and output (rec) jets   
// *******************************************

#include  "AliAnalysisTaskSE.h"
#include  "THnSparse.h" // cannot forward declare ThnSparseF
#include  "AliTHn.h" // cannot forward declare 

////////////////
class AliJetHeader;
class AliESDEvent;
class AliAODEvent;
class AliAODExtension;
class AliAODJet;
class AliVParticle;
class AliAODJetEventBackground;
class AliGenPythiaEventHeader;
class AliCFManager;
class AliCFContainer;

class TList;
class TChain;
class TH1F;
class TH2F;
class TH3F;
class TRandom3;
class TProfile;
class TProfile2D;
class TSTring;

class AliAnalysisTaskJetSpectrum2 : public AliAnalysisTaskSE
{
 public:
    AliAnalysisTaskJetSpectrum2();
    AliAnalysisTaskJetSpectrum2(const char* name);
    virtual ~AliAnalysisTaskJetSpectrum2();
    // Implementation of interface methods
    virtual void UserCreateOutputObjects();
    virtual void Init();
    virtual void LocalInit() { Init(); }
    virtual void UserExec(Option_t *option);
    virtual void Terminate(Option_t *option);
    virtual Bool_t Notify();

    virtual void SetUseGlobalSelection(Bool_t b){fUseGlobalSelection = b;}
    virtual void SetEventClass(Int_t i){fEventClass = i;}
    virtual void SetExternalWeight(Float_t f){fExternalWeight = f;}
    virtual void SetUseExternalWeightOnly(Bool_t b){fUseExternalWeightOnly = b;}
    virtual void SetAODJetInput(Bool_t b){fUseAODJetInput = b;}
    virtual void SetAODTrackInput(Bool_t b){fUseAODTrackInput = b;}
    virtual void SetAODMCInput(Bool_t b){fUseAODMCInput = b;}
    virtual void SetLimitGenJetEta(Bool_t b){fLimitGenJetEta = b;}
    virtual void SetJetEtaWindow(Float_t f){fJetRecEtaWindow = f;}
    virtual void SetTrackEtaWindow(Float_t f){fTrackRecEtaWindow = f;}
    virtual void SetNMatchJets(Short_t f){fNMatchJets = f;}
    virtual void SetMinJetPt(Float_t f){fMinJetPt = f;}
    virtual void SetNRPBins(Short_t i){fNRPBins = i;} 
    virtual void SetTRP(Int_t i){fTRP=i;}
    virtual void SetDebug(Int_t i){fDebug=i;}
    virtual void SetNMultBins(Short_t i) {fNBinsMult=i;}
    virtual void SetNPtLeadingBins(Short_t i) {fNBinsLeadingTrackPt=i;}
    virtual void SetFlagJetType(Int_t iType,Int_t iF){fFlagJetType[iType] = iF;}
    virtual void SetMinTrackPt(Float_t f){fMinTrackPt = f;}
    virtual void SetDeltaPhiWindow(Float_t f){fDeltaPhiWindow = f;}
    virtual void SetAnalysisType(Int_t i){fAnalysisType = i;}
    virtual void SetBranchGen(const char* c){fBranchGen = c;}
    virtual void SetBranchRec(const char* c){fBranchRec = c;}
    virtual void SetBranchBkgRec(const char* c){fBranchBkgRec = c;}  
    virtual void SetBranchBkgGen(const char* c){fBranchBkgGen = c;}  
    virtual void SetTrackTypeGen(Int_t i){fTrackTypeGen = i;}
    virtual void SetTrackTypeRec(Int_t i){fTrackTypeRec = i;}
    virtual void SetFilterMask(UInt_t i){fFilterMask = i;}
    virtual void SetJetTriggerExclude(UInt_t i){fJetTriggerExcludeMask = i;}
    virtual void SetJetTriggerBest(UInt_t i){fJetTriggerBestMask = i;}
    virtual void SetMatching(Bool_t b = kTRUE){fDoMatching = b;}
    virtual void SetRPMethod(Int_t i){fRPMethod = i;}
    virtual void SetEventSelectionMask(UInt_t i){fEventSelectionMask = i;}

    virtual void SetNonStdFile(char* c){fNonStdFile = c;} 

    virtual void SetNTrigger(Int_t n);
    virtual void SetTrigger(Int_t i,UInt_t it,const char* c = "");


    virtual void SetNAcceptance(Int_t n);
    virtual void SetAcceptance(Int_t i,Float_t phiMin,Float_t phiMax,Float_t etaMin,Float_t etaMax);// non overlapping regions
    virtual Int_t CheckAcceptance(Float_t phi,Float_t eta);


    void    SetMaxVertexZ(Float_t a)      {fMaxVertexZ = a;}
    void    SetMinNcontributors(Int_t i)  {fMinNcontributors = i;}
    void    SetPileupRejection(Bool_t a)  {fRejectPileup = a;}



    // Helper
    //

    // we have different cases
    // AOD reading -> MC from AOD
    // ESD reading -> MC from Kinematics
    // this has to match with our selection of input events
    enum {kTrackUndef = 0, kTrackAOD, kTrackKineAll,kTrackKineCharged, kTrackAODMCAll, kTrackAODMCCharged, kTrackAODMCChargedAcceptance};
    enum {kAnaMC =  0x1, kAnaMCESD = 0x2};
    enum {kMaxJets = 2};
    enum {kJetRec = 0, kJetGen, kJetRecFull, kJetGenFull, kJetTypes}; //
    enum {kMaxCorrelation =  3};
    
    // 
    // Stored as function of generated values
    // 0 all gen jets
    // 1 all gen jets in eta window
    // 2 all gen jets with rec partner
    // 3 all gen jets in eta window with rec partner
    // 4 all gen jets in eta window with rec partner in eta window
    // 5 all gen jets in eta window with rec partner in eta window with leading track on reconstructed level
    // 6 all rec jets in eta window with gen partner

    // Stored as function of reconstructed values:
    // 7 all rec jets in eta window


    enum {kStep0 = 0, kStep1, kStep2, kStep3, kStep4, kStep5, kStep6, kStep7,kMaxStep};


 private:

    AliAnalysisTaskJetSpectrum2(const AliAnalysisTaskJetSpectrum2&);
    AliAnalysisTaskJetSpectrum2& operator=(const AliAnalysisTaskJetSpectrum2&);

    void    MakeJetContainer();
    Int_t   GetListOfTracks(TList *list,Int_t type);
    void    FillTrackHistos(TList &particlesList,int iType);
    Float_t GetRho(TList &list);
    Float_t GetCentrality();
    Bool_t  CalculateReactionPlaneAngle(const TList *trackList);
    Double_t RelativePhi(Double_t phi1,Double_t phi2);
   
 
    Int_t   GetPhiBin(Double_t phi);
    Double_t GetPhiWeight(Double_t phi,Double_t signedpt);
    Int_t   GetListOfJets(TList *list,TClonesArray* jarray,Int_t type);
    void    FillJetHistos(TList &jetsList,TList &particlesList,Int_t iType);

    void    FillMatchHistos(TList &recJetsList,TList &genJetsList);

    Bool_t  JetSelected(AliAODJet *jet);
    Int_t MultFromJetRefs(TClonesArray *jets);
    AliVParticle *LeadingTrackFromJetRefs(AliAODJet* jet);
    AliVParticle *LeadingTrackInCone(AliAODJet* jet,TList *list,Float_t r = 0.4);

    AliJetHeader *fJetHeaderRec;//! The jet header that can be fetched from the userinfo
    AliJetHeader *fJetHeaderGen;//! The jet header that can fetched from the userinfo
    AliAODEvent  *fAODIn; //! where we take the jets from 
    AliAODEvent  *fAODOut; //! where we take the jets from 
    AliAODExtension  *fAODExtension; //! where we take the jets from can be input or output AOD

    AliCFContainer *fhnJetContainer; //! like particle container in corrfw with different steps need AliCFContainer with Scale(), and clone() to do the same
    THnSparse    *fhnCorrelation;    //! response matrix for unfolding 
    THnSparseF   *fhnEvent;          //! event counts 
    TF1          *f1PtScale;         //! correction function to correct to the average true jet energy depending on p_T,rec

    TString       fBranchRec;  // AOD branch name for reconstructed
    TString       fBranchGen;  // AOD brnach for genereated
    TString       fBranchBkgRec;  //AOD branch for background 
    TString       fBranchBkgGen;  //AOD branch for background 
    TString       fNonStdFile; // name of delta aod file to catch the extension

    TRandom3*     fRandomizer; //! randomizer

    Bool_t        fUseAODJetInput;        // take jet from input AOD not from ouptu AOD
    Bool_t        fUseAODTrackInput;      // take track from input AOD not from ouptu AOD
    Bool_t        fUseAODMCInput;         // take MC from input AOD not from ouptu AOD
    Bool_t        fUseGlobalSelection;    // Limit the eta of the generated jets
    Bool_t        fUseExternalWeightOnly; // use only external weight
    Bool_t        fLimitGenJetEta;        // Limit the eta of the generated jets
    Bool_t        fDoMatching;            // switch on the matching between rec and gen
    Short_t       fNMatchJets;            // number of leading jets considered from the list
    Short_t       fNRPBins;               // number of bins with respect to RP
    Int_t         fTRP;                   //flag to bin the RC in |phi_RP-phi
    Int_t         fDebug;      
    UInt_t        fJetTriggerExcludeMask; // mask for jet triggers to exclude
    UInt_t        fJetTriggerBestMask; // mask for best jet triggers
    UInt_t        fFilterMask;            // filter bit for slecected tracks
    UInt_t        fEventSelectionMask;    // Selection information used to filter events
    Int_t         fNTrigger;              // number of triggers for selection
    UInt_t        *fTriggerBit;            //[fNTrigger] trigger bits 
    Int_t         fNAcceptance;            // number of triggers for selection
    Short_t       fNBinsLeadingTrackPt;   // number of bins leading track pt in sparse. Two options: 1 or 10
    Short_t       fNBinsMult;             // number of bins in multiplicity in sparse
    Int_t         fAnalysisType;          // Analysis type 
    Int_t         fTrackTypeRec;          // type of tracks used for FF 
    Int_t         fTrackTypeGen;          // type of tracks used for FF 
    Int_t         fFlagJetType[kJetTypes]; // disable the filling and booking of certain JetType histos
    Int_t         fEventClass;            // event class to be looked at for this instance of the task
    Int_t         fRPMethod;              // method for subevent calculation
    Float_t       fAvgTrials;             // Average nimber of trials
    Float_t       fExternalWeight;        // external weight
    Float_t       fJetRecEtaWindow;       // eta window for rec jets
    Float_t       fTrackRecEtaWindow;     // eta window for rec tracks
    Float_t       fMinJetPt;              // limits the jet p_T in addition to what already is done in the jet finder, this is important for jet matching for JF with lo threshold
    Float_t       fMinTrackPt;            // limits the track p_T 
    Float_t       fDeltaPhiWindow;        // minium angle between dijets
    Float_t       *fAcceptancePhiMin;     //[fNAcceptance] minimum phi 
    Float_t       *fAcceptancePhiMax;     //[fNAcceptance] maximum phi
    Float_t       *fAcceptanceEtaMin;     //[fNAcceptance] minimum eta 
    Float_t       *fAcceptanceEtaMax;     //[fNAcceptance] maximum eta
    Float_t       fCentrality;            // ! centrality
    Float_t       fRPAngle;               // ! RP angle of the reaction plane
    Int_t         fMultRec;               // ! reconstructed track multiplicity
    Int_t         fMultGen;               // ! generated track multiplicity
    TString       *fTriggerName;  //[fNTrigger] array of trigger names

    TProfile*     fh1Xsec;   //! pythia cross section and trials
    TH1F*         fh1Trials; //! trials are added
    TH1F*         fh1AvgTrials; //! trials are added
    TH1F*         fh1PtHard;  //! Pt har of the event...       
    TH1F*         fh1PtHardNoW;  //! Pt har of the event without weigt       
    TH1F*         fh1PtHardTrials;  //! Number of trials 
    TH1F*         fh1ZVtx;          //! z-vtx distribution
    TH1F*         fh1RP;            //! RP distribution
    TH1F*         fh1Centrality;    //! centrality distribution
    TH1F*         fh1TmpRho;        //! just temporary histo for calculation    
    TH2F*         fh2MultRec;       //! reconstructed track multiplicity   
    TH2F*         fh2MultGen;       //! generated track multiplicity   
    TH2F*         fh2RPCentrality;   //! RP vs centrality

    TH2F*         fh2PtFGen;                //! found vs generated 
    TH3F*         fh2deltaPt1Pt2;           //! ptjet1 - ptjet2 vs ptjet1 vs ptjet2
    TH2F*         fh2RelPtFGen;             //! relative difference between generated and found 
    TH3F*         fh3RelPtFGenLeadTrkPt;    //! relative difference between generated and found vs leading track pT

    

    // Jet histos second go

    TH1F*         fh1NJets[kJetTypes];      //! nr of gen jets
    TH1F*         fh1SumPtTrack[kJetTypes]; //! sum over all track pT    

    TH1F*         fh1PtIn[kJetTypes][kMaxJets+1];  //! Jet pt  
    TH1F*         fh1PtJetsIn[kJetTypes];       //! Jet pt for all jets
    TH1F*         fh1PtJetsInRej[kJetTypes];    //! Jet pt for all rejected jets
    TH1F*         fh1PtJetsInBest[kJetTypes];    //! Jet pt for all rejected jets
    TH1F*         fh1PtTracksIn[kJetTypes];     //! track pt for all tracks
    TH1F*         fh1PtTracksInLow[kJetTypes];  //! track pt for all tracks
    
    TH2F*         fh2NJetsPt[kJetTypes];    //! Number of found jets above threshold
    TH2F*         fh2NTracksPt[kJetTypes];  //! Number of tracks above threshold
    TProfile2D    *fp2MultRPPhiTrackPt[kJetTypes];         //! for mean pT vs RP   
    TProfile2D    *fp2CentRPPhiTrackPt[kJetTypes];         //! for mean pT vs RP   
    THnSparseF    *fhnJetPt[kJetTypes];                  //! jet pt information for analysis
    THnSparseF    *fhnJetPtBest[kJetTypes];              //! best jet for analysis
    THnSparseF    *fhnJetPtRej[kJetTypes];               //! Rej jet for analysis
    THnSparseF    *fhnJetPtQA[kJetTypes];                //! jet pt information for QA
    THnSparseF    *fhnTrackPt[kJetTypes];                //! track pt information for analysis
    THnSparseF    *fhnTrackPtQA[kJetTypes];              //! track pt information for analysis

    TH2F*         fh2LTrackPtJetPt[kJetTypes][kMaxJets+1];       //! leading track within the jet vs jet pt 

    TH1F*   fh1DijetMinv[kJetTypes];            //! dijet inv mass
    TH2F*   fh2DijetDeltaPhiPt[kJetTypes];      //! dijet delta phi vs pt
    TH2F*   fh2DijetAsymPt[kJetTypes];          //! dijet asym vs pt after delta phi cut
    TH2F*   fh2DijetPt2vsPt1[kJetTypes];        //! dijet pt2 vs pt1
    TH2F*   fh2DijetDifvsSum[kJetTypes];        //! dijet dif vs sum

    TH1F*   fh1EvtSelection;
    Float_t fMaxVertexZ;
    Int_t   fMinNcontributors;
    Bool_t  fRejectPileup;


    TList *fHistList;                  //! Output list
   

    ClassDef(AliAnalysisTaskJetSpectrum2, 23); // Analysis task for standard jet analysis
};
 
#endif
