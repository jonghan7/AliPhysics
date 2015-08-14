
/**************************************************************************
 * Copyright(c) 1998-2010, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

/////////////////////////////////////////////////////////////
//
// Class to handle systematic errors for charm hadrons
//
// Usage:
// AliHFSystEff syst;           // DECAY = 1 for D0, 2, for D+, 3 for D*, 5 for Lc->pKp, 6 for Lc->pK0S
// syst.SetRunNumber(YEAR);     // YEAR = two last numbers of the year (is 10 for 2010)
// syst.SetCollisionType(TYPE);  // TYPE =  0 is pp, 1 is PbPb
// syst.SetCentrality(CENT);     // CENT is centrality, 0100 for MB, 020 (4080) for 0-20 (40-80) CC...
// syst.Init(DECAY);             // DECAY = 1 for D0, 2, for D+, 3 for D* 
// syst.DrawErrors(); // to see a plot of the error contributions
// syst.GetTotalSystErr(pt); // to get the total err at pt 
//
// Author: A.Dainese, andrea.dainese@pd.infn.it
/////////////////////////////////////////////////////////////

#include <TStyle.h>
#include <TGraphAsymmErrors.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TLegend.h>
#include <TColor.h>

#include "AliLog.h"
#include "AliHFSystErr.h"

/// \cond CLASSIMP
ClassImp(AliHFSystErr);
/// \endcond

//--------------------------------------------------------------------------
AliHFSystErr::AliHFSystErr(const Char_t* name, const Char_t* title) : 
TNamed(name,title),
fNorm(0),
fRawYield(0),
fTrackingEff(0),
fBR(0),
fCutsEff(0),
fPIDEff(0),
fMCPtShape(0),
fPartAntipart(0),
fRunNumber(10),
fCollisionType(0),
fCentralityClass("0100"),
fRapidityRange("0101"),
fIsLowEnergy(false),
fIsLowPtAnalysis(false),
fIsCentScan(false),
fIsRapidityScan(false)
{
  //
  /// Default Constructor
  //
}

//--------------------------------------------------------------------------
AliHFSystErr::~AliHFSystErr() {
  //  
  /// Default Destructor
  //
  /*

  if(fNorm)         { delete fNorm; fNorm=0; }
  if(fRawYield)     { delete fRawYield; fRawYield=0; }
  if(fTrackingEff)  { delete fTrackingEff; fTrackingEff=0; }
  if(fBR)           { delete fBR; fBR=0; }
  if(fCutsEff)      { delete fCutsEff; fCutsEff=0; }
  if(fPIDEff)       { delete fPIDEff; fPIDEff=0; }
  if(fMCPtShape)    { delete fMCPtShape; fMCPtShape=0; }
  if(fPartAntipart) { delete fPartAntipart; fPartAntipart=0; }
  */
}

//--------------------------------------------------------------------------
void AliHFSystErr::Init(Int_t decay){
  //
  /// Variables/histos initialization
  //

  if ((fRunNumber>11) && fIsLowEnergy==false) { 
    AliFatal("Only settings for 2010 and the low energy runs are implemented so far");
  }

  switch(decay) {
  case 1: // D0->Kpi
    if (fCollisionType==0) {
      if (fIsLowEnergy) InitD0toKpi2010ppLowEn();
      else if(fIsLowPtAnalysis) InitD0toKpi2010ppLowPtAn();
      else InitD0toKpi2010pp();
    } 
    else if (fCollisionType==1) {
      if (fRunNumber == 10){
	if (fCentralityClass=="010") InitD0toKpi2010PbPb010CentScan();
	else if (fCentralityClass=="1020") InitD0toKpi2010PbPb1020CentScan();
	else if (fCentralityClass=="020")  InitD0toKpi2010PbPb020();
	else if (fCentralityClass=="2040") InitD0toKpi2010PbPb2040CentScan();
	else if (fCentralityClass=="4060") InitD0toKpi2010PbPb4060CentScan();
	else if (fCentralityClass=="6080") InitD0toKpi2010PbPb6080CentScan();
	else if (fCentralityClass=="4080") InitD0toKpi2010PbPb4080();
	else AliFatal("Not yet implemented");
      }
      else if (fRunNumber == 11){
	if (fCentralityClass=="07half") InitD0toKpi2011PbPb07half();
	else if (fCentralityClass=="010") InitD0toKpi2011PbPb010();
	else if (fCentralityClass=="3050InPlane") InitD0toKpi2011PbPb3050InPlane();
	else if (fCentralityClass=="3050OutOfPlane")InitD0toKpi2011PbPb3050OutOfPlane();
	else if (fCentralityClass == "3050")InitD0toKpi2011PbPb3050();
	else if (fCentralityClass=="010" && fIsCentScan) InitD0toKpi2011PbPb010CentScan();
	else if (fCentralityClass=="1020") InitD0toKpi2011PbPb1020CentScan();
	else if (fCentralityClass=="2030") InitD0toKpi2011PbPb2030CentScan();
	else if (fCentralityClass=="3040") InitD0toKpi2011PbPb3040CentScan();
	else if (fCentralityClass=="4050") InitD0toKpi2011PbPb4050CentScan();
	else if (fCentralityClass=="5080") InitD0toKpi2010PbPb5080CentScan();
	else AliFatal("Not yet implemented");
      }
    } 
    else if (fCollisionType==2) { 
      if (fCentralityClass=="0100"){
	if(fIsLowPtAnalysis) InitD0toKpi2013pPb0100LowPtAn();
	else InitD0toKpi2013pPb0100();
      }
      if (fCentralityClass=="020V0A") InitD0toKpi2013pPb020V0A();
      if (fCentralityClass=="2040V0A") InitD0toKpi2013pPb2040V0A();
      if (fCentralityClass=="4060V0A") InitD0toKpi2013pPb4060V0A();
      if (fCentralityClass=="60100V0A") InitD0toKpi2013pPb60100V0A();
        
      if (fCentralityClass=="020ZNA") InitD0toKpi2013pPb020ZNA();
      if (fCentralityClass=="2040ZNA") InitD0toKpi2013pPb2040ZNA();
      if (fCentralityClass=="4060ZNA") InitD0toKpi2013pPb4060ZNA();
      if (fCentralityClass=="60100ZNA") InitD0toKpi2013pPb60100ZNA();
      
      if (fCentralityClass=="020CL1") InitD0toKpi2013pPb020CL1();
      if (fCentralityClass=="2040CL1") InitD0toKpi2013pPb2040CL1();
      if (fCentralityClass=="4060CL1") InitD0toKpi2013pPb4060CL1();
      if (fCentralityClass=="60100CL1") InitD0toKpi2013pPb60100CL1();
      
      if (fIsRapidityScan) {
	if (fRapidityRange == "0804") InitD0toKpi2013pPb0100RapScan0804();
	if (fRapidityRange == "0401") InitD0toKpi2013pPb0100RapScan0401();
	if (fRapidityRange == "0101") InitD0toKpi2013pPb0100RapScan0101();
	if (fRapidityRange == "0104") InitD0toKpi2013pPb0100RapScan0104();
	if (fRapidityRange == "0408") InitD0toKpi2013pPb0100RapScan0408();
      }
    }
    else AliFatal("Not yet implemented");
    break;
    
  case 2: // D+->Kpipi
    if(fIsLowPtAnalysis) AliFatal("Not yet implemented");
    if (fCollisionType==0) {
      if (fIsLowEnergy) InitDplustoKpipi2010ppLowEn();
      else InitDplustoKpipi2010pp();
    } 
    else if (fCollisionType==1) {
      if(fIsLowPtAnalysis) AliFatal("Not yet implemented");
      if (fRunNumber == 10){
	if (fCentralityClass=="010") InitDplustoKpipi2010PbPb010CentScan();
	else if (fCentralityClass=="1020") InitDplustoKpipi2010PbPb1020CentScan();
	else if (fCentralityClass=="020") InitDplustoKpipi2010PbPb020();
	else if (fCentralityClass=="2040") InitDplustoKpipi2010PbPb2040CentScan();
	else if (fCentralityClass=="4060") InitDplustoKpipi2010PbPb4060CentScan();
	else if (fCentralityClass=="6080") InitDplustoKpipi2010PbPb6080CentScan();
	else if (fCentralityClass=="4080") InitDplustoKpipi2010PbPb4080();
	else AliFatal("Not yet implemented");
      }
      if(fRunNumber == 11){
	if (fCentralityClass=="07half") InitDplustoKpipi2011PbPb07half();
	else if (fCentralityClass=="010") InitDplustoKpipi2011PbPb010();
	else if (fCentralityClass=="010" && fIsCentScan) InitDplustoKpipi2011PbPb010CentScan();
	else if (fCentralityClass=="1020") InitDplustoKpipi2011PbPb1020CentScan();
	else if (fCentralityClass=="2030") InitDplustoKpipi2011PbPb2030CentScan();
	else if (fCentralityClass=="3040") InitDplustoKpipi2011PbPb3040CentScan();
	else if (fCentralityClass=="4050") InitDplustoKpipi2011PbPb4050CentScan();
	else if (fCentralityClass=="5080") InitDplustoKpipi2010PbPb5080CentScan();
    else if (fCentralityClass=="3050") InitDplustoKpipi2011PbPb3050();
	else AliFatal("Not yet implemented");
      }
    } 
    else if (fCollisionType==2) { 
      if (fCentralityClass=="0100") InitDplustoKpipi2013pPb0100();
      
      if (fCentralityClass=="020V0A") InitDplustoKpipi2013pPb020V0A();
      if (fCentralityClass=="2040V0A") InitDplustoKpipi2013pPb2040V0A();
      if (fCentralityClass=="4060V0A") InitDplustoKpipi2013pPb4060V0A();
      if (fCentralityClass=="60100V0A") InitDplustoKpipi2013pPb60100V0A();
      
      if (fCentralityClass=="020ZNA") InitDplustoKpipi2013pPb020ZNA();
      if (fCentralityClass=="2040ZNA") InitDplustoKpipi2013pPb2040ZNA();
      if (fCentralityClass=="4060ZNA") InitDplustoKpipi2013pPb4060ZNA();
      if (fCentralityClass=="60100ZNA") InitDplustoKpipi2013pPb60100ZNA();
      
      
      if (fCentralityClass=="020CL1") InitDplustoKpipi2013pPb020CL1();
      if (fCentralityClass=="2040CL1") InitDplustoKpipi2013pPb2040CL1();
      if (fCentralityClass=="4060CL1") InitDplustoKpipi2013pPb4060CL1();
      if (fCentralityClass=="60100CL1") InitDplustoKpipi2013pPb60100CL1();
      
      
      if (fIsRapidityScan) {
	if (fRapidityRange == "0804") InitDplustoKpipi2013pPb0100RapScan0804();
	if (fRapidityRange == "0401") InitDplustoKpipi2013pPb0100RapScan0401();
	if (fRapidityRange == "0101") InitDplustoKpipi2013pPb0100RapScan0101();
	if (fRapidityRange == "0104") InitDplustoKpipi2013pPb0100RapScan0104();
	if (fRapidityRange == "0408") InitDplustoKpipi2013pPb0100RapScan0408();
      }
    } 
    else AliFatal("Not yet implemented");
    break;
  case 3: // D*->D0pi
    if(fIsLowPtAnalysis) AliFatal("Not yet implemented");
    if (fCollisionType==0) {
      if(fIsLowEnergy)  InitDstartoD0pi2010ppLowEn();
      else InitDstartoD0pi2010pp();
    }
    else if (fCollisionType==1) {
      if (fRunNumber == 10){
	if (fCentralityClass=="010") InitDstartoD0pi2010PbPb010CentScan();
	else if (fCentralityClass=="1020") InitDstartoD0pi2010PbPb1020CentScan();
	else if (fCentralityClass=="020") InitDstartoD0pi2010PbPb020();
	else if (fCentralityClass=="2040" && fIsCentScan) InitDstartoD0pi2010PbPb2040CentScan();
	else if (fCentralityClass=="2040") InitDstartoD0pi2010PbPb2040();
	else if (fCentralityClass=="4060") InitDstartoD0pi2010PbPb4060CentScan();
	else if (fCentralityClass=="6080") InitDstartoD0pi2010PbPb6080CentScan();
	else if (fCentralityClass=="4080") InitDstartoD0pi2010PbPb4080();
	else AliFatal("Not yet implemented");
      }
      if (fRunNumber == 11){
	if (fCentralityClass=="07half") InitDstartoD0pi2011PbPb07half();
	else if (fCentralityClass=="010") InitDstartoD0pi2011PbPb010();
	else if (fCentralityClass=="010" && fIsCentScan) InitDstartoD0pi2011PbPb010CentScan();
	else if (fCentralityClass=="1020") InitDstartoD0pi2011PbPb1020CentScan();
	else if (fCentralityClass=="2030") InitDstartoD0pi2011PbPb2030CentScan();
	else if (fCentralityClass=="3040") InitDstartoD0pi2011PbPb3040CentScan();
	else if (fCentralityClass=="4050") InitDstartoD0pi2011PbPb4050CentScan();
	else if (fCentralityClass=="5080") InitDstartoD0pi2010PbPb5080CentScan();
    else if (fCentralityClass=="3050") InitDstartoD0pi2011PbPb3050();
	else AliFatal("Not yet implemented");
      }
    }
    else if (fCollisionType==2) { 
      if (fCentralityClass=="0100") InitDstartoD0pi2013pPb0100();
        
      if (fCentralityClass=="020V0A") InitDstartoD0pi2013pPb020V0A();
      if (fCentralityClass=="2040V0A") InitDstartoD0pi2013pPb2040V0A();
      if (fCentralityClass=="4060V0A") InitDstartoD0pi2013pPb4060V0A();
      if (fCentralityClass=="60100V0A") InitDstartoD0pi2013pPb60100V0A();
      
      if (fCentralityClass=="020ZNA") InitDstartoD0pi2013pPb020ZNA();
      if (fCentralityClass=="2040ZNA") InitDstartoD0pi2013pPb2040ZNA();
      if (fCentralityClass=="4060ZNA") InitDstartoD0pi2013pPb4060ZNA();
      if (fCentralityClass=="60100ZNA") InitDstartoD0pi2013pPb60100ZNA();
      
      if (fCentralityClass=="020CL1") InitDstartoD0pi2013pPb020CL1();
      if (fCentralityClass=="2040CL1") InitDstartoD0pi2013pPb2040CL1();
      if (fCentralityClass=="4060CL1") InitDstartoD0pi2013pPb4060CL1();
      if (fCentralityClass=="60100CL1") InitDstartoD0pi2013pPb60100CL1();
      
      if (fIsRapidityScan) {
        if (fRapidityRange == "0804") InitDstartoD0pi2013pPb0100RapScan0804();
        if (fRapidityRange == "0401") InitDstartoD0pi2013pPb0100RapScan0401();
        if (fRapidityRange == "0101") InitDstartoD0pi2013pPb0100RapScan0101();
        if (fRapidityRange == "0104") InitDstartoD0pi2013pPb0100RapScan0104();
        if (fRapidityRange == "0408") InitDstartoD0pi2013pPb0100RapScan0408();
      }
    }
    else AliFatal("Not yet implemented");
    break;
  case 4: // D+s->KKpi
    if(fIsLowPtAnalysis) AliFatal("Not yet implemented");
    if (fCollisionType==0) InitDstoKKpi2010pp();
    else if (fCollisionType==1) {
      if (fCentralityClass=="07half") InitDstoKKpi2011PbPb07half();
      else if (fCentralityClass=="010") InitDstoKKpi2011PbPb010();
      else if (fCentralityClass=="2050") InitDstoKKpi2011PbPb2050();
      else AliFatal("Not yet implemented");
    }
    else if (fCollisionType==2) { 
      if (fCentralityClass=="0100") InitDstoKKpi2013pPb0100();
    }
    else AliFatal("Not yet implemented");
    break;
  case 5: // Lc->pKpi
    if (fCollisionType==0) InitLctopKpi2010pp();
    else AliFatal("Not yet implemented");
    break;
  case 6: // Lc->pK0S
    if (fCollisionType==0) InitLctopK0S2010pp();
    else AliFatal("Not yet implemented");
    break;
    
  default:
    printf("Invalid decay type: %d\n",decay);
    break;
  }

}
  
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010pp() {
  // 
  // D0->Kpi syst errors. Responsible: A. Rossi
  //   2010 pp sample
  //

  // Normalization
  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.035); // 4% error on sigmaV0and

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",24,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.012); // 1.2% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.08); // 8% (4% per track)

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,1);
  fRawYield->SetBinContent(2,0.22);
  fRawYield->SetBinContent(3,0.1);
  for(Int_t i=4;i<=7;i++) fRawYield->SetBinContent(i,0.04);
  for(Int_t i=8;i<=12;i++) fRawYield->SetBinContent(i,0.07);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,1);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fCutsEff->SetBinContent(i,0.10); // 10%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.03); // 3%
  fPIDEff->SetBinContent(2,0.05); // 5%

  // MC dN/dpt
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0);
  fMCPtShape->SetBinContent(1,0.03);
  fMCPtShape->SetBinContent(2,0.03);

  // particle-antiparticle
  //  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",24,0,24);
  //  fPartAntipart->SetBinContent(1,1); 
  //  for(Int_t i=2;i<=24;i++) fPartAntipart->SetBinContent(i,0.05);
  
  return;
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010PbPb020() {
  // 
  // D0->Kpi syst errors. Responsible: A. Rossi
  //   2010 PbPb sample, 0-20 CC
  //

  // Normalization
  fNorm = new TH1F("fNorm","fNorm",20,0,20);
  for(Int_t i=1;i<=20;i++) fNorm->SetBinContent(i,0.05); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",20,0,20);
  for(Int_t i=1;i<=20;i++) fBR->SetBinContent(i,0.012); // 1.2% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fTrackingEff->SetBinContent(i,0.10);// Jacek, 5% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",20,0,20);
  fRawYield->SetBinContent(1,0);
  fRawYield->SetBinContent(2,0);
  fRawYield->SetBinContent(3,0.08);
  for(Int_t i=4;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.10);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",20,0,20);
  fCutsEff->SetBinContent(1,0.);
  fCutsEff->SetBinContent(2,0.);
  fCutsEff->SetBinContent(3,0.13);
  fCutsEff->SetBinContent(4,0.11);  
  for(Int_t i=5;i<=16;i++) fCutsEff->SetBinContent(i,0.10);
  for(Int_t i=17;i<=20;i++) fCutsEff->SetBinContent(i,0.);

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",20,0,20);
  for(Int_t i=3;i<=16;i++) fPIDEff->SetBinContent(i,0.05);

  // MC dN/dpt
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",20,0,20);
  for(Int_t i=1;i<=20;i++) fMCPtShape->SetBinContent(i,0.01);
  fMCPtShape->SetBinContent(3,0.04);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=13;i<=16;i++) fMCPtShape->SetBinContent(i,0.03); 

//   // particle-antiparticle
//   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
//   for(Int_t i=3;i<=12;i++) fPartAntipart->SetBinContent(i,0.05);
//   fPartAntipart->SetBinContent(3,0.10);
//   fPartAntipart->SetBinContent(4,0.10);
//   fPartAntipart->SetBinContent(7,0.10);
//   fPartAntipart->SetBinContent(8,0.10);
  
  return;
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2011PbPb07half() {
  // 
  // D0->Kpi syst errors. Responsible: A. Rossi
  //   2011 PbPb sample, 0-7.5 CC
  //

  // Normalization
  fNorm = new TH1F("fNorm","fNorm",36,0,36);
  for(Int_t i=1;i<36;i++) fNorm->SetBinContent(i,0.048); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",36,0,36);
  for(Int_t i=1;i<=36;i++) fBR->SetBinContent(i,0.012); // 1.2% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",36,0,36);
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.10);// Jacek, 5% per track
  for(Int_t i=25;i<=36;i++) fTrackingEff->SetBinContent(i,0.);// OUT OF MEASUREMENT RANGE

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",36,0,36);
  fRawYield->SetBinContent(1,0);
  fRawYield->SetBinContent(2,0.1);
  fRawYield->SetBinContent(3,0.05);
  for(Int_t i=4;i<=12;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.30);
  for(Int_t i=25;i<=36;i++) fRawYield->SetBinContent(i,0.);// OUT OF MEASUREMENT RANGE

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",36,0,36);
  fCutsEff->SetBinContent(1,0.);
  fCutsEff->SetBinContent(2,0.15);
  fCutsEff->SetBinContent(3,0.13);
  fCutsEff->SetBinContent(4,0.11);  
  fCutsEff->SetBinContent(5,0.08); 
  for(Int_t i=6;i<=24;i++) fCutsEff->SetBinContent(i,0.06);
  for(Int_t i=25;i<=36;i++) fCutsEff->SetBinContent(i,0.0);// OUT OF MEASUREMENT RANGE

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",36,0,36);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.05);
  for(Int_t i=25;i<=36;i++) fPIDEff->SetBinContent(i,0.0);// OUT OF MEASUREMENT RANGE

  // MC dN/dpt
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",36,0,36);
  for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  fMCPtShape->SetBinContent(2,0.06);
  fMCPtShape->SetBinContent(3,0.04);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=13;i<=16;i++) fMCPtShape->SetBinContent(i,0.03); 
  for(Int_t i=17;i<=24;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=25;i<=36;i++) fMCPtShape->SetBinContent(i,0.0);// OUT OF MEASUREMENT RANGE

//   // particle-antiparticle
//   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",36,0,36);
//   for(Int_t i=3;i<=12;i++) fPartAntipart->SetBinContent(i,0.05);
//   fPartAntipart->SetBinContent(3,0.10);
//   fPartAntipart->SetBinContent(4,0.10);
//   fPartAntipart->SetBinContent(7,0.10);
//   fPartAntipart->SetBinContent(8,0.10);
  
  return;
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2011PbPb3050InPlane() {
  //
  // D0->Kpi syst errors. Responsible: D. Caffarri
  //   2011 PbPb sample, 30-50 CC InPlane
  //
  InitD0toKpi2011PbPb07half();
  // Raw yield extraction
  // fRawYield = new TH1F("fRawYield","fRawYield",36,0,36);
  fRawYield->SetBinContent(1,0);
  fRawYield->SetBinContent(2,0);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.08);
  for(Int_t i=5;i<=8;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.15);
  for(Int_t i=17;i<=36;i++) fRawYield->SetBinContent(i,0.);// OUT OF MEASUREMENT RANGE

  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",36,0,36);
  fCutsEff->SetBinContent(1,0.);
  fCutsEff->SetBinContent(2,0.0);
  fCutsEff->SetBinContent(3,0.10);
  fCutsEff->SetBinContent(4,0.10);
  fCutsEff->SetBinContent(5,0.10);
  fCutsEff->SetBinContent(6,0.10);
  for(Int_t i=7;i<=8;i++) fCutsEff->SetBinContent(i,0.15);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.15);
  for(Int_t i=25;i<=36;i++) fCutsEff->SetBinContent(i,0.0);// OUT OF MEASUREMENT RANGE
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2011PbPb3050OutOfPlane() {
  //
  // D0->Kpi syst errors. Responsible: D. Caffarri
  //   2011 PbPb sample, 30-50 CC OutOfPlane
  //
  InitD0toKpi2011PbPb07half();
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",36,0,36);
  fRawYield->SetBinContent(1,0);
  fRawYield->SetBinContent(2,0.);
  fRawYield->SetBinContent(3,0.05);
  for(Int_t i=4;i<=6;i++) fRawYield->SetBinContent(i,0.07);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.15);
  for(Int_t i=17;i<=36;i++) fRawYield->SetBinContent(i,0.);// OUT OF MEASUREMENT RANGE

  // Cuts efficiency (from cuts variation)
  // fCutsEff = new TH1F("fCutsEff","fCutsEff",36,0,36);
  fCutsEff->SetBinContent(1,0.);
  fCutsEff->SetBinContent(2,0.);
  fCutsEff->SetBinContent(3,0.10);
  fCutsEff->SetBinContent(4,0.10);
  fCutsEff->SetBinContent(5,0.10);
  fCutsEff->SetBinContent(6,0.10);
  for(Int_t i=7;i<=8;i++) fCutsEff->SetBinContent(i,0.15);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.15);
  for(Int_t i=17;i<=36;i++) fCutsEff->SetBinContent(i,0.0);// OUT OF MEASUREMENT RANGE
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010PbPb4080() {
  //
  // D0->Kpi syst errors. Responsible: A. Rossi
  //   2010 PbPb sample, 40-80 CC
  //

  // Normalization
  fNorm = new TH1F("fNorm","fNorm",20,0,20);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.07); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",20,0,20);
  for(Int_t i=1;i<=20;i++) fBR->SetBinContent(i,0.012); // 1.2% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fTrackingEff->SetBinContent(i,0.10); // Jacek, 5% per track


  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",20,0,20);
  fRawYield->SetBinContent(1,0);
  fRawYield->SetBinContent(2,0);
  for(Int_t i=3;i<=16;i++) fRawYield->SetBinContent(i,0.05);
  //for(Int_t i=13;i<=20;i++) fRawYield->SetBinContent(i,0);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",20,0,20);
  fCutsEff->SetBinContent(1,0.);
  fCutsEff->SetBinContent(2,0.);
  fCutsEff->SetBinContent(3,0.13);
  fCutsEff->SetBinContent(4,0.11);  
  for(Int_t i=5;i<=16;i++) fCutsEff->SetBinContent(i,0.10);
  for(Int_t i=17;i<=20;i++) fCutsEff->SetBinContent(i,0.);

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",20,0,20);
//   for(Int_t i=3;i<=6;i++) fPIDEff->SetBinContent(i,0.10);
//   for(Int_t i=7;i<=16;i++) fPIDEff->SetBinContent(i,0.05);
  for(Int_t i=3;i<=16;i++) fPIDEff->SetBinContent(i,0.05);

  // MC dN/dpt
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",20,0,20);
  for(Int_t i=1;i<=20;i++) fMCPtShape->SetBinContent(i,0.01);
  //  fMCPtShape->SetBinContent(3,0.04); Not set for peripherals (Raa Vs pt is flat)
  //  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=13;i<=16;i++) fMCPtShape->SetBinContent(i,0.03); 
  
  //   // particle-antiparticle
  //   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
  //   for(Int_t i=3;i<=12;i++) fPartAntipart->SetBinContent(i,0.05);
  
  return;
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2011PbPb010() {
    //
    // D0->Kpi syst errors. Responsible: A. Rossi
    //   2011 PbPb sample, 0-10 CC
    //

  AliInfo(" Settings for D0 --> Kpi Pb-Pb collisions at 2.76 TeV - 0-10 centrality - newww!!");
    
    // Normalization
    fNorm = new TH1F("fNorm","fNorm",24,0,24);
    for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.048); // TAA and pp norm
    
    // Branching ratio
    fBR = new TH1F("fBR","fBR",24,0,24);
    for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.012); // 1.2% PDG2010
    
    // Tracking efficiency
    fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);
    for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.10); // Jacek, 5% per track
    
    
    // Raw yield extraction
    fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
    fRawYield->SetBinContent(1,0);
    fRawYield->SetBinContent(2,0.15);
    fRawYield->SetBinContent(3,0.09);
    fRawYield->SetBinContent(4,0.06);
    fRawYield->SetBinContent(5,0.06);
    fRawYield->SetBinContent(6,0.06);
    for (Int_t i=7; i<=8; i++) fRawYield->SetBinContent(i,0.06);
    for (Int_t i=9; i<=12; i++)fRawYield->SetBinContent(i,0.07);
    for (Int_t i=13; i<=16; i++) fRawYield->SetBinContent(i,0.11);
    for (Int_t i=17; i<=24; i++) fRawYield->SetBinContent(i,0.30);
    
    fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
    fCutsEff->SetBinContent(1,0.);
    fCutsEff->SetBinContent(2,0.15);
    fCutsEff->SetBinContent(3,0.12);
    fCutsEff->SetBinContent(4,0.05);
    fCutsEff->SetBinContent(5,0.05);
    fCutsEff->SetBinContent(6,0.05);
    for (Int_t i=7; i<=8; i++) fCutsEff->SetBinContent(i,0.05);
    for (Int_t i=9; i<=12; i++)fCutsEff->SetBinContent(i,0.05);
    for (Int_t i=13; i<=16; i++) fCutsEff->SetBinContent(i,0.05);
    for (Int_t i=17; i<=24; i++) fCutsEff->SetBinContent(i,0.05);

    
    // PID efficiency (from PID/noPID)
    fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
    //   for(Int_t i=3;i<=6;i++) fPIDEff->SetBinContent(i,0.10);
    //   for(Int_t i=7;i<=16;i++) fPIDEff->SetBinContent(i,0.05);
    for(Int_t i=2;i<=16;i++) fPIDEff->SetBinContent(i,0.05);
    
    // MC dN/dpt
    fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
    fMCPtShape->SetBinContent(1,0.);
    fMCPtShape->SetBinContent(2,0.15);
    fMCPtShape->SetBinContent(3,0.12);
    fMCPtShape->SetBinContent(4,0.02);
    for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.);
  
    //   // particle-antiparticle
    //   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
    //   for(Int_t i=3;i<=12;i++) fPartAntipart->SetBinContent(i,0.05);
    
    return;
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2011PbPb3050() {
    //
    // D0->Kpi syst errors. Responsible: D. Caffarri
    //   2011 PbPb sample, 30-50 CC
    //
    
    // Normalization
    fNorm = new TH1F("fNorm","fNorm",20,0,20);
    for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.07); // TAA and pp norm
    
    // Branching ratio
    fBR = new TH1F("fBR","fBR",20,0,20);
    for(Int_t i=1;i<=20;i++) fBR->SetBinContent(i,0.012); // 1.2% PDG2010
    
    // Tracking efficiency
    fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",20,0,20);
    for(Int_t i=1;i<=20;i++) fTrackingEff->SetBinContent(i,0.10); // Jacek, 5% per track
    
    
    // Raw yield extraction
    fRawYield = new TH1F("fRawYield","fRawYield",20,0,20);
    fRawYield->SetBinContent(1,0);
    fRawYield->SetBinContent(2,0.102);
    fRawYield->SetBinContent(3,0.071);
    fRawYield->SetBinContent(4,0.052);
    fRawYield->SetBinContent(5,0.063);
    fRawYield->SetBinContent(6,0.071);
    fRawYield->SetBinContent(7,0.071);
    fRawYield->SetBinContent(8,0.071);
    fRawYield->SetBinContent(9,0.071);
    fRawYield->SetBinContent(10,0.071);
    fRawYield->SetBinContent(11,0.071);
    fRawYield->SetBinContent(12,0.071);
    
    for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.112);
    for(Int_t i=17;i<=20;i++) fRawYield->SetBinContent(i,0);
    
    fCutsEff = new TH1F("fCutsEff","fCutsEff",20,0,20);
    fCutsEff->SetBinContent(1,0.);
    fCutsEff->SetBinContent(2,0.10);
    fCutsEff->SetBinContent(3,0.10);
    fCutsEff->SetBinContent(4,0.10);
    fCutsEff->SetBinContent(5,0.10);
    fCutsEff->SetBinContent(6,0.10);
    for(Int_t i=7;i<=8;i++) fCutsEff->SetBinContent(i,0.15);
    for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.15);

    
    // PID efficiency (from PID/noPID)
    fPIDEff = new TH1F("fPIDEff","fPIDEff",20,0,20);
    //   for(Int_t i=3;i<=6;i++) fPIDEff->SetBinContent(i,0.10);
    //   for(Int_t i=7;i<=16;i++) fPIDEff->SetBinContent(i,0.05);
    for(Int_t i=2;i<=16;i++) fPIDEff->SetBinContent(i,0.05);
    
    // MC dN/dpt
    fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",0,0,20);
    for(Int_t i=1;i<=20;i++) fMCPtShape->SetBinContent(i,0.01);
    fMCPtShape->SetBinContent(2,0.06);
    fMCPtShape->SetBinContent(3,0.04);
    fMCPtShape->SetBinContent(4,0.02);
    for(Int_t i=13;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
    for(Int_t i=17;i<=20;i++) fMCPtShape->SetBinContent(i,0.);
    //   // particle-antiparticle
    //   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
    //   for(Int_t i=3;i<=12;i++) fPartAntipart->SetBinContent(i,0.05);
    
    return;
}


//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010ppLowEn() {
  // 
  // D0->Kpi syst errors. Low energy run
  //   2011 2.76 TeV pp sample
  //
  AliInfo(" Settings for D0 --> K pi, p-p collisions at 2.76 TeV"); 

  // Normalization
  fNorm = new TH1F("fNorm","fNorm",20,0,20);
  for(Int_t i=1;i<=20;i++) fNorm->SetBinContent(i,0.019); //  error on sigmaV0and

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",20,0,20);
  for(Int_t i=1;i<=20;i++) fBR->SetBinContent(i,0.012); // 1.2% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fTrackingEff->SetBinContent(i,0.10); //10% (5% per track)

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",20,0,20);
  fRawYield->SetBinContent(1,1);
  for(Int_t i=1;i<=20;i++) fRawYield->SetBinContent(i,0.15);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fCutsEff->SetBinContent(i,0.10); // 10% 
  fCutsEff->SetBinContent(2,0.20);
  for(Int_t i=7;i<=20;i++) fCutsEff->SetBinContent(i,0.15); // 10% 


  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fPIDEff->SetBinContent(i,0.15); // 10%
  //  fPIDEff->SetBinContent(2,0.20);
  for(Int_t i=7;i<=20;i++) fPIDEff->SetBinContent(i,0.05); // 10%

  // MC dN/dpt
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",20,0,20);
  for(Int_t i=1;i<=20;i++) fMCPtShape->SetBinContent(i,0.01);
  fMCPtShape->SetBinContent(1,0.03);
  fMCPtShape->SetBinContent(2,0.03);

//   // particle-antiparticle
//   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
//   fPartAntipart->SetBinContent(1,1);
//   fPartAntipart->SetBinContent(2,1);
//   for(Int_t i=3;i<=6;i++) fPartAntipart->SetBinContent(i,0.08);
//   for(Int_t i=1;i<=20;i++) fPartAntipart->SetBinContent(i,0.);
  
  return;
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010ppLowPtAn() {
  // 
  // D0->Kpi syst errors. Responsible: 
  //   2010 pp sample, analysis without topological cuts
  //

  AliInfo(" Settings for D0 --> K pi, pp collisions at 7 TeV, analysis without topological cuts"); 

  // Normalization
  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.035); // 4% error on sigmaV0and

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",24,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.0129); // (0.05/3.88) 

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.08); // 8% (4% per track)

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  for(Int_t i=1;i<=24;i++) fRawYield->SetBinContent(i,0.2);
  fRawYield->SetBinContent(1,0.14);
  fRawYield->SetBinContent(2,0.14);
  fRawYield->SetBinContent(3,0.10);
  fRawYield->SetBinContent(4,0.10);
  fRawYield->SetBinContent(5,0.10);
  fRawYield->SetBinContent(6,0.14);
  fRawYield->SetBinContent(7,0.18);
  fRawYield->SetBinContent(8,0.18);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.2);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fCutsEff->SetBinContent(i,0.); 

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.03); // 3%
  fPIDEff->SetBinContent(1,0.05); // 5%
  fPIDEff->SetBinContent(2,0.05); // 5%

  // MC dN/dpt
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0);

  // particle-antiparticle
  //  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",24,0,24);
  //  for(Int_t i=1;i<=24;i++) fPartAntipart->SetBinContent(i,0.);
  
  return;
}
//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb0100(){
  //
  // D0->Kpi syst errors. p-Pb data sample
  //
  AliInfo(" Settings for D0 --> K pi, p-Pb collisions at 5.023 TeV"); 

  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.02); // 
  
  // Branching ratio 
  fBR = new TH1F("fBR","fBR",24,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.013); // 1.2% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.06); // 3% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.08);
  for(Int_t i=3;i<=12;i++) fRawYield->SetBinContent(i,0.04);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.07);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.11);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.08);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);

  // MC dN/dpt  
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.02);
  for(Int_t i=3;i<=24;i++) fMCPtShape->SetBinContent(i,0.0);

  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
  for(Int_t i=1;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */
  return;


}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb0100LowPtAn(){
  //
  // D0->Kpi syst errors. p-Pb data sample
  // analysis without topological cuts

  AliInfo(" Settings for D0 --> K pi, p-Pb collisions at 5.023 TeV, analysis without topological cuts"); 

  // Normalization
  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.035); // 4% error on sigmaV0and

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",24,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.0129); // (0.05/3.88) 

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.06); // 3% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  for(Int_t i=1;i<=24;i++) fRawYield->SetBinContent(i,0.2);
  fRawYield->SetBinContent(1,0.15);
  fRawYield->SetBinContent(2,0.15);
  fRawYield->SetBinContent(3,0.10);
  fRawYield->SetBinContent(4,0.10);
  fRawYield->SetBinContent(5,0.10);
  fRawYield->SetBinContent(6,0.10);
  fRawYield->SetBinContent(7,0.10);
  fRawYield->SetBinContent(8,0.10);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.11);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fCutsEff->SetBinContent(i,0.); 

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.); 

  // MC dN/dpt
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0);

  // particle-antiparticle
  //  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",24,0,24);
  //  for(Int_t i=1;i<=24;i++) fPartAntipart->SetBinContent(i,0.);

  return;


}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2010pp() {
  // 
  // D+->Kpipi syst errors. Responsible: R. Bala
  //  2010 pp sample
  //


// Normalization
  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.035); // 4% error on sigmaV0and

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",24,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.021); // 2.1% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.12); // 12% (4% per track)


  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,1);
  fRawYield->SetBinContent(2,0.25);
  fRawYield->SetBinContent(3,0.25);
  fRawYield->SetBinContent(4,0.20);
  fRawYield->SetBinContent(5,0.09);
  fRawYield->SetBinContent(6,0.09);
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.05);  
  for(Int_t i=12;i<=24;i++) fRawYield->SetBinContent(i,0.10);  
  
  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fCutsEff->SetBinContent(i,0.10); // 10%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.05); // 5%
  fPIDEff->SetBinContent(1,0.15); // 15%
  fPIDEff->SetBinContent(2,0.15); // 15%
  fPIDEff->SetBinContent(3,0.15); // 15%
  fPIDEff->SetBinContent(4,0.15); // 15%
  for(Int_t i=12;i<=16;i++) fPIDEff->SetBinContent(i,0.10); // 5%

  // MC dN/dpt  
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0);
  fMCPtShape->SetBinContent(1,0.03);
  fMCPtShape->SetBinContent(2,0.03);


  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
  fPartAntipart->SetBinContent(1,1);
  fPartAntipart->SetBinContent(2,1);
  fPartAntipart->SetBinContent(3,0.12);
  for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */
  return;
}
 
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstoKKpi2010pp() {
  // 
  // D+s->KKpi syst errors. Responsible: G.M. Innocenti
  //  2010 pp sample
  //


// Normalization
  fNorm = new TH1F("fNorm","fNorm",12,0,12);
  for(Int_t i=1;i<=12;i++) fNorm->SetBinContent(i,0.035); // 3.5% error on sigmaV0and

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",12,0,12);
  for(Int_t i=1;i<=12;i++) fBR->SetBinContent(i,0.06); // 0.14/2.32 PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fTrackingEff->SetBinContent(i,0.12); // 12% (4% per track)


  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",12,0,12);
  fRawYield->SetBinContent(1,1);
  fRawYield->SetBinContent(2,1);
  fRawYield->SetBinContent(3,0.20);
  fRawYield->SetBinContent(4,0.20);
  fRawYield->SetBinContent(5,0.15);
  fRawYield->SetBinContent(6,0.15);
  fRawYield->SetBinContent(7,0.15);
  fRawYield->SetBinContent(8,0.15);
  fRawYield->SetBinContent(9,0.20);
  fRawYield->SetBinContent(10,0.20);
  fRawYield->SetBinContent(11,0.20);
  fRawYield->SetBinContent(12,0.20);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fCutsEff->SetBinContent(i,0.15); // 15%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fPIDEff->SetBinContent(i,0.07); // 7%

  // MC dN/dpt 
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",12,0,12);
  for(Int_t i=1; i<=2; i++) fMCPtShape->SetBinContent(i,1.);
  for(Int_t i=3; i<=4; i++) fMCPtShape->SetBinContent(i,0.03);
  for(Int_t i=5; i<=6; i++) fMCPtShape->SetBinContent(i,0.03);
  for(Int_t i=7; i<=8; i++) fMCPtShape->SetBinContent(i,0.02);
  for(Int_t i=9; i<=12; i++) fMCPtShape->SetBinContent(i,0.02);


  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",12,0,12);
  fPartAntipart->SetBinContent(1,1);
  fPartAntipart->SetBinContent(2,1);
  fPartAntipart->SetBinContent(3,0.12);
  for(Int_t i=4;i<=12;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */
  return;
}
   
 
//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2011PbPb07half() {
  // 
  // D+->Kpipi syst errors. Responsible: E. Bruna
  //  2011 PbPb sample, 0-7.5% CC
  //

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",36,0,36);
  for(Int_t i=1;i<=36;i++) fNorm->SetBinContent(i,0.048); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",36,0,36);
  for(Int_t i=1;i<=36;i++) fBR->SetBinContent(i,0.021); // 2.1% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",36,0,36);
  for(Int_t i=1;i<=36;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",36,0,36);
  for(Int_t i=1;i<=36;i++) fRawYield->SetBinContent(i,.10);  //5 to 10%
  fRawYield->SetBinContent(4,0.30);
  fRawYield->SetBinContent(5,0.20);
  fRawYield->SetBinContent(6,0.20);
  for(Int_t i=7; i<=8; i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=9; i<=12; i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=13; i<=16; i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=17; i<=24; i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=25; i<=36; i++) fRawYield->SetBinContent(i,0.20);


  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",36,0,36);
  for(Int_t i=1; i<=12; i++) fCutsEff->SetBinContent(i,0.10);
  for(Int_t i=13; i<=36; i++) fCutsEff->SetBinContent(i,0.08);


  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",36,0,36);
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.05); // 5%

  // MC dN/dpt  (24/7/2012)
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",36,0,36);
  for(Int_t iBin=1; iBin<=5; iBin++) fMCPtShape->SetBinContent(iBin,0.05);
  for(Int_t iBin=6; iBin<=36; iBin++) fMCPtShape->SetBinContent(iBin,0.03);
  //  for(Int_t iBin=13; iBin<=36; iBin++) fMCPtShape->SetBinContent(iBin,0.05);


  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
  fPartAntipart->SetBinContent(1,1);
  fPartAntipart->SetBinContent(2,1);
  fPartAntipart->SetBinContent(3,0.12);
  for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */

  return;
}
//_________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2011PbPb010() {
  // 
  // D+->Kpipi syst errors. Responsible: E. Bruna, C. Bedda 
  //  2011 PbPb sample, 0-10% CC
  //

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",36,0,36);
  for(Int_t i=1;i<=36;i++) fNorm->SetBinContent(i,0.048); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",36,0,36);
  for(Int_t i=1;i<=36;i++) fBR->SetBinContent(i,0.021); // 2.1% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",36,0,36);
  for(Int_t i=1;i<=36;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",36,0,36);
  for(Int_t i=1;i<=3;i++) fRawYield->SetBinContent(i,0.);  //5 to 10%
  fRawYield->SetBinContent(4,0.10);
  fRawYield->SetBinContent(5,0.08);
  fRawYield->SetBinContent(6,0.08);
  for(Int_t i=7; i<=8; i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=9; i<=12; i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=13; i<=16; i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17; i<=24; i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=25; i<=36; i++) fRawYield->SetBinContent(i,0.08);


  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",36,0,36);
  for(Int_t i=1; i<=36; i++) fCutsEff->SetBinContent(i,0.10);


  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",36,0,36);
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.05); // 5%

  // MC dN/dpt 
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",36,0,36);
  for(Int_t i=1;i<=3;i++) fMCPtShape->SetBinContent(i,0.); 
  fMCPtShape->SetBinContent(4,0.06);
  fMCPtShape->SetBinContent(5,0.02);
  for (Int_t i=6; i<=36; i++) fMCPtShape->SetBinContent(i,0.01);
  //  for(Int_t iBin=13; iBin<=36; iBin++) fMCPtShape->SetBinContent(iBin,0.05);


  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
  fPartAntipart->SetBinContent(1,1);
  fPartAntipart->SetBinContent(2,1);
  fPartAntipart->SetBinContent(3,0.12);
  for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */

  return;
}

//___________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2011PbPb3050() {
    //
    // D+->Kpipi syst errors. Responsible: R. Bala
    //  2011 PbPb sample, 30-50% CC
    //
    AliInfo(" Settings for D+ --> K pi pi, Pb-Pb collisions for 30-50 CC");
    // Normalization
    fNorm = new TH1F("fNorm","fNorm",16,0,16);
    for(Int_t i=1;i<=16;i++) fNorm->SetBinContent(i,0.048); // TAA and pp norm
    
    // Branching ratio
    fBR = new TH1F("fBR","fBR",16,0,16);
    for(Int_t i=1;i<=16;i++) fBR->SetBinContent(i,0.021); // 2.1% PDG2010
    
    // Tracking efficiency
    fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",16,0,16);
    for(Int_t i=1;i<=36;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track
    
    // Raw yield extraction
    fRawYield = new TH1F("fRawYield","fRawYield",16,0,16);
    for(Int_t i=1;i<=12;i++) fRawYield->SetBinContent(i,0.09);  //5 to 10%
    for(Int_t i=13; i<=16; i++) fRawYield->SetBinContent(i,0.12);
    

    // Cuts efficiency (from cuts variation)
    fCutsEff = new TH1F("fCutsEff","fCutsEff",16,0,16);
    for(Int_t i=1; i<=12; i++) fCutsEff->SetBinContent(i,0.10);
    for(Int_t i=12; i<=16; i++) fCutsEff->SetBinContent(i,0.14);
    
    // PID efficiency (from PID/noPID)
    fPIDEff = new TH1F("fPIDEff","fPIDEff",16,0,16);
    for(Int_t i=1;i<=16;i++) fPIDEff->SetBinContent(i,0.05); // 5%
    
    // MC dN/dpt  (24/7/2012)
    fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",16,0,16);
    fMCPtShape->SetBinContent(3,0.10);
    fMCPtShape->SetBinContent(4,0.04);
    for(Int_t iBin=5; iBin<=16; iBin++) fMCPtShape->SetBinContent(iBin,0.02);
    
    
    // particle-antiparticle
    /*
     fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
     fPartAntipart->SetBinContent(1,1);
     fPartAntipart->SetBinContent(2,1);
     fPartAntipart->SetBinContent(3,0.12);
     for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
     */
    
    return;
}


//--------------------------------------------------------------------------
void AliHFSystErr::InitDstoKKpi2011PbPb07half() {
  // 
  // D+s->Kpipi syst errors. Responsible: G.M. Innocenti
  //  2011 PbPb sample, 0-7.5% CC
  //

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",12,0,12);
  for(Int_t i=1;i<=12;i++) fNorm->SetBinContent(i,0.048); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",12,0,12);
  for(Int_t i=1;i<=12;i++) fBR->SetBinContent(i,0.053); // 0.12/2.28 PDG2012

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",12,0,12);
  for(Int_t i=1;i<=6;i++) fRawYield->SetBinContent(i,.30); 
  for(Int_t i=7; i<=12; i++) fRawYield->SetBinContent(i,0.20);
 


  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fCutsEff->SetBinContent(i,0.20); // 20%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fPIDEff->SetBinContent(i,0.1); // 10%

   // MC dN/dpt 
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",12,0,12);
  for(Int_t i=1; i<=2; i++) fMCPtShape->SetBinContent(i,1.);
  for(Int_t i=3; i<=4; i++) fMCPtShape->SetBinContent(i,0.03);
  for(Int_t i=5; i<=6; i++) fMCPtShape->SetBinContent(i,0.03);
  for(Int_t i=7; i<=8; i++) fMCPtShape->SetBinContent(i,0.02);
  for(Int_t i=9; i<=12; i++) fMCPtShape->SetBinContent(i,0.02);

  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",12,0,12);
  fPartAntipart->SetBinContent(1,1);
  fPartAntipart->SetBinContent(2,1);
  fPartAntipart->SetBinContent(3,0.12);
  for(Int_t i=4;i<=12;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */

  return;
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDstoKKpi2011PbPb010() {
  // 
  // D+s->Kpipi syst errors. Responsible: A.Barbano
  //  2011 PbPb sample, 0-10% CC
  //

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",12,0,12);
  for(Int_t i=1;i<=12;i++) fNorm->SetBinContent(i,0.047); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",12,0,12);
  for(Int_t i=1;i<=12;i++) fBR->SetBinContent(i,0.045); // PDG2015

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fTrackingEff->SetBinContent(i,0.15); // 5% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",12,0,12);
  for(Int_t i=1;i<=6;i++) fRawYield->SetBinContent(i,0.15); // 15% 
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.10); // 10% 


  // Cuts efficiency (from cuts variation + phi mass selection cuts)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fCutsEff->SetBinContent(i,0.20); // 20%  

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fPIDEff->SetBinContent(i,0.10); // 10%

   // MC dN/dpt 
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",12,0,12);
  for(Int_t i=1; i<=6; i++) fMCPtShape->SetBinContent(i,0.02);  //2%
  for(Int_t i=7; i<=8; i++) fMCPtShape->SetBinContent(i,0.01);  //1%
  for(Int_t i=9; i<=12; i++) fMCPtShape->SetBinContent(i,0.01); //1%

  return;
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDstoKKpi2011PbPb2050() {
  // 
  // D+s->Kpipi syst errors. Responsible: A.Barbano
  //  2011 PbPb sample, 20-50% CC
  //

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",12,0,12);
  for(Int_t i=1;i<=12;i++) fNorm->SetBinContent(i,0.05); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",12,0,12);
  for(Int_t i=1;i<=12;i++) fBR->SetBinContent(i,0.045); // PDG2015

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fTrackingEff->SetBinContent(i,0.15); // 5% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",12,0,12);
  for(Int_t i=1;i<=12;i++) fRawYield->SetBinContent(i,.15); //15%  


  // Cuts efficiency (from cuts variation + phi selection cuts)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fCutsEff->SetBinContent(i,0.20); // 20%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",12,0,12);
  for(Int_t i=1;i<=12;i++) fPIDEff->SetBinContent(i,0.10); // 10%

   // MC dN/dpt 
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",12,0,12);
  for(Int_t i=1; i<=12; i++) fMCPtShape->SetBinContent(i,0.01); //1%

  return;
}

//_________________________________________________________________________
void AliHFSystErr::InitDstoKKpi2013pPb0100(){
  //
  // Dstar syst
  AliInfo(" Settings for Ds --> K K pi, p-Pb collisions at 5.023 TeV"); 
  
  //Normalization
  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=3;i<=12;i++) fNorm->SetBinContent(i,0.00); // 
  
 // Branching ratio 
  fBR = new TH1F("fBR","fBR",12,0,12);
  for(Int_t i=3;i<=12;i++) fBR->SetBinContent(i,0.053); // 0.12/2.28 PDG2012

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);
  for(Int_t i=3;i<=12;i++) fTrackingEff->SetBinContent(i,0.09); // 3% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  for(Int_t i=3; i<=4; i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=5; i<=6; i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=7; i<=8; i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9; i<=12; i++) fRawYield->SetBinContent(i,0.08);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  for(Int_t i=3;i<=12;i++) fCutsEff->SetBinContent(i,0.15); //

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=3;i<=4;i++) fPIDEff->SetBinContent(i,0.10); 
  for(Int_t i=5;i<=12;i++) fPIDEff->SetBinContent(i,0.05); 

  // MC dN/dpt  
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=3;i<=12;i++) fMCPtShape->SetBinContent(i,0.04);

  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
  fPartAntipart->SetBinContent(1,1);
  fPartAntipart->SetBinContent(2,1);
  fPartAntipart->SetBinContent(3,0.12);
  for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */
  return;
}


//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2010PbPb020() {
  // 
  // D+->Kpipi syst errors. Responsible: ??
  //  2010 PbPb sample, 0-20 CC
  //

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",20,0,20);
  for(Int_t i=1;i<=20;i++) fNorm->SetBinContent(i,0.05); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",20,0,20);
  for(Int_t i=1;i<=20;i++) fBR->SetBinContent(i,0.021); // 2.1% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",20,0,20);
  for(Int_t i=1;i<=20;i++) fRawYield->SetBinContent(i,.10);  //5 to 10%
  // fRawYield->SetBinContent(5,0.23);
  //fRawYield->SetBinContent(6,0.23);
  fRawYield->SetBinContent(7,0.20);
  fRawYield->SetBinContent(8,0.20);
  fRawYield->SetBinContent(9,0.15);
  fRawYield->SetBinContent(10,0.15);
  fRawYield->SetBinContent(11,0.15);
  fRawYield->SetBinContent(12,0.15);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fCutsEff->SetBinContent(i,0.15); // 10%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fPIDEff->SetBinContent(i,0.05); // 5%

  // MC dN/dpt  (2/2/2012)
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",20,0,20);
  for(Int_t i=1;i<=20;i++) fMCPtShape->SetBinContent(i,0.);
  for(Int_t iBin=7; iBin<=8; iBin++) fMCPtShape->SetBinContent(iBin,0.01);
  for(Int_t iBin=9; iBin<=12; iBin++) fMCPtShape->SetBinContent(iBin,0.05);
  for(Int_t iBin=13; iBin<=16; iBin++) fMCPtShape->SetBinContent(iBin,0.05);


  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
  fPartAntipart->SetBinContent(1,1);
  fPartAntipart->SetBinContent(2,1);
  fPartAntipart->SetBinContent(3,0.12);
  for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */

  return;
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2010PbPb4080() {
  // 
  // D+->Kpipi syst errors. Responsible: ??
  //  2010 PbPb sample, 40-80 CC
  //
  

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",20,0,20);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.07); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",20,0,20);
  for(Int_t i=1;i<=20;i++) fBR->SetBinContent(i,0.021); // 2.1% 

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track


  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",20,0,20);
  fRawYield->SetBinContent(1,1);
  fRawYield->SetBinContent(2,1);
  fRawYield->SetBinContent(3,1);
  fRawYield->SetBinContent(4,0.15);
  fRawYield->SetBinContent(5,0.05);
  fRawYield->SetBinContent(6,0.05);
  fRawYield->SetBinContent(7,0.15);
  fRawYield->SetBinContent(8,0.15);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.15);
  for(Int_t i=13;i<=20;i++) fRawYield->SetBinContent(i,1);  //5 to 10%

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fCutsEff->SetBinContent(i,0.10); // 10%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fPIDEff->SetBinContent(i,0.05); // 5%
  fPIDEff->SetBinContent(3,0.13); // 13%
 

  // MC dN/dpt  (2/2/2012)
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",20,0,20);
  for(Int_t i=1;i<=20;i++) fMCPtShape->SetBinContent(i,0);
  for(Int_t iBin=4; iBin<=8; iBin++) fMCPtShape->SetBinContent(iBin,0.01);
  for(Int_t iBin=9; iBin<=12; iBin++) fMCPtShape->SetBinContent(iBin,0.03);
  for(Int_t iBin=13; iBin<=16; iBin++) fMCPtShape->SetBinContent(iBin,0.03);


  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
  fPartAntipart->SetBinContent(1,1);
  fPartAntipart->SetBinContent(2,1);
  fPartAntipart->SetBinContent(3,0.12);
  for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */
  return;
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2010ppLowEn() {

  // 
  // D+->Kpipi syst errors. Responsible: R. Bala
  //  2011 2.76 TeV pp sample
  //
  AliInfo(" Settings for D+ --> K pi pi p-p collisions at 2.76 TeV"); 

  // Normalization
  fNorm = new TH1F("fNorm","fNorm",20,0,20);
  for(Int_t i=1;i<=20;i++) fNorm->SetBinContent(i,0.019); // error on sigmaV0and

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",20,0,20);
  for(Int_t i=1;i<=20;i++) fBR->SetBinContent(i,0.021); // 2.1% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fTrackingEff->SetBinContent(i,0.15); // 3% (1% per track)

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",20,0,20);
  fRawYield->SetBinContent(1,1);
  fRawYield->SetBinContent(2,1);
  for(Int_t i=3;i<=6;i++) fRawYield->SetBinContent(i,0.10);  //5 to 10%
  fRawYield->SetBinContent(7,0.15);
  fRawYield->SetBinContent(8,0.15); 
  for(Int_t i=9;i<=20;i++) fRawYield->SetBinContent(i,0.055);  //5 to 10%

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fCutsEff->SetBinContent(i,0.15); // 10%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fPIDEff->SetBinContent(i,0.05); // 5%
  fPIDEff->SetBinContent(3,0.10); // 13%
  fPIDEff->SetBinContent(4,0.10); // 13%
 
  // MC dN/dpt  (copied from D0 : will update later)
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",20,0,20);
  for(Int_t i=1;i<=20;i++) fMCPtShape->SetBinContent(i,0.01);
  fMCPtShape->SetBinContent(1,0.03);
  fMCPtShape->SetBinContent(2,0.03);

  return;
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb0100(){
  //
  // Dplus syst 
  AliInfo(" Settings for D+ --> K pi pi, p-Pb collisions at 5.023 TeV"); 

  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.02); // 
  
  fBR = new TH1F("fBR","fBR",20,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.021); // 2.1% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.09); // 3% per track
  
  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.);
  fRawYield->SetBinContent(2,0.1);
  fRawYield->SetBinContent(3,0.08);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.05);
  fRawYield->SetBinContent(6,0.05);
  fRawYield->SetBinContent(7,0.05);
  fRawYield->SetBinContent(8,0.05);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.08);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.);
  fCutsEff->SetBinContent(2,0.1);
  fCutsEff->SetBinContent(3,0.1);
  fCutsEff->SetBinContent(4,0.06);
  fCutsEff->SetBinContent(5,0.06);
  fCutsEff->SetBinContent(6,0.06);
  fCutsEff->SetBinContent(7,0.05);
  fCutsEff->SetBinContent(8,0.05);
  for(Int_t i=9;i<=12;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=13;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=17;i<=24;i++) fCutsEff->SetBinContent(i,0.05);

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  fPIDEff->SetBinContent(1,1);
  fPIDEff->SetBinContent(2,0.1);
  for(Int_t i=3;i<=24;i++) fPIDEff->SetBinContent(i,0.0); //

  // MC dN/dpt
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0.0);

  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
  fPartAntipart->SetBinContent(1,1);
  fPartAntipart->SetBinContent(2,1);
  fPartAntipart->SetBinContent(3,0.12);
  for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */
  return;


}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010pp() {
  // 
  // D*+->D0pi syst errors. Responsible: A. Grelli, Y. Wang
  //  2010 pp sample
  //

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.035); // 4% error on sigmaV0and

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",24,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.015); // 1.5% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);
  fTrackingEff->SetBinContent(1,1.0);
  fTrackingEff->SetBinContent(2,0.13); // 10% (ITSsa) \oplus 8% (4% per ITSTPC track)
  fTrackingEff->SetBinContent(3,0.12);
  fTrackingEff->SetBinContent(3,0.12);
  for(Int_t i=4;i<=24;i++) fTrackingEff->SetBinContent(i,0.12); // 12% (4% per track)


  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,1.0);
  fRawYield->SetBinContent(2,0.10);
  fRawYield->SetBinContent(3,0.04);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.05);
  fRawYield->SetBinContent(7,0.05);
  fRawYield->SetBinContent(8,0.05);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.04);  //4%
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);  //4%
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.2);  //4%

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(2,0.22);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.10); // 10%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.04); // 3%
 

  // MC dN/dpt  (copied from D0 : will update later)
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0);
  fMCPtShape->SetBinContent(1,0.03);
  fMCPtShape->SetBinContent(2,0.03);

  return;


}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010ppLowEn() {

  // 
  // D+->Kpipi syst errors. Responsible: A. Grelli
  //  2011 2.76 TeV pp sample
  //
  AliInfo(" Settings for D*+ --> D0 pi p-p collisions at 2.76 TeV"); 

// Normalization
  fNorm = new TH1F("fNorm","fNorm",20,0,20);
  for(Int_t i=1;i<=20;i++) fNorm->SetBinContent(i,0.019); // error on sigmaV0and

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",20,0,20);
  for(Int_t i=1;i<=20;i++) fBR->SetBinContent(i,0.015); // 1.5% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fTrackingEff->SetBinContent(i,0.15); //10% (to be checked!!)

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",20,0,20);
  fRawYield->SetBinContent(1,1);
  fRawYield->SetBinContent(2,1);
  fRawYield->SetBinContent(3,0.14);
  fRawYield->SetBinContent(4,0.14);
  fRawYield->SetBinContent(5,0.12);
  fRawYield->SetBinContent(6,0.12);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  fRawYield->SetBinContent(9,0.08);
  fRawYield->SetBinContent(10,0.08);
  fRawYield->SetBinContent(11,0.08);
  fRawYield->SetBinContent(12,0.08);
  for(Int_t i=9;i<=20;i++) fRawYield->SetBinContent(i,0.065);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fCutsEff->SetBinContent(i,0.10);  
  fCutsEff->SetBinContent(3,0.15);
  fCutsEff->SetBinContent(4,0.15);
  fCutsEff->SetBinContent(5,0.15);
  fCutsEff->SetBinContent(6,0.15);
  fCutsEff->SetBinContent(7,0.10);
  fCutsEff->SetBinContent(8,0.10);
  fCutsEff->SetBinContent(9,0.10);
  fCutsEff->SetBinContent(10,0.10);
  fCutsEff->SetBinContent(11,0.10);
  fCutsEff->SetBinContent(12,0.10);

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",20,0,20);
  for(Int_t i=1;i<=20;i++) fPIDEff->SetBinContent(i,0.05); // 10%

  // MC dN/dpt
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",20,0,20);
  for(Int_t i=1;i<=20;i++) fMCPtShape->SetBinContent(i,0.01);
  fMCPtShape->SetBinContent(1,0.03);
  fMCPtShape->SetBinContent(2,0.03);

  return;
}

//------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010PbPb020() {
  // 
  // D*+->D0pi syst errors. Responsible: A. Grelli
  //  2010 PbPb sample, 0-20 CC
  //

  AliInfo(" Settings for D*+ --> D0pi Pb-Pb collisions at 2.76 TeV - 0-20 centrality - DUMMY"); 

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=20;i++) fNorm->SetBinContent(i,0.05); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",24,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.015); // 1.5% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);;
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track


  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  for(Int_t i=1;i<=24;i++) fRawYield->SetBinContent(i,0.1);  //4%
  fRawYield->SetBinContent(3,0.2);
  fRawYield->SetBinContent(4,0.2);
  fRawYield->SetBinContent(5,0.2);
  fRawYield->SetBinContent(6,0.2);
 
  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fCutsEff->SetBinContent(i,0.10); // 10%
  fCutsEff->SetBinContent(4,0.15);
  fCutsEff->SetBinContent(5,0.15);
  fCutsEff->SetBinContent(6,0.15);

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.05); // 3%
 

  // MC dN/dpt  (from study on D* pt shape)
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0.045);
  fMCPtShape->SetBinContent(4,0.025);
  fMCPtShape->SetBinContent(5,0.025);
  fMCPtShape->SetBinContent(6,0.025);
  fMCPtShape->SetBinContent(7,0.04);
  fMCPtShape->SetBinContent(8,0.04);
  fMCPtShape->SetBinContent(9,0.03);
  fMCPtShape->SetBinContent(10,0.03);
  fMCPtShape->SetBinContent(11,0.03);
  fMCPtShape->SetBinContent(12,0.03);
  
  return;

}
// ----------------------------- 2011 ----------------------------------
void AliHFSystErr::InitDstartoD0pi2011PbPb07half() {
  // 
  // D*+->D0pi syst errors. Responsible: A. Grelli
  //  2011 PbPb sample, 0-7.5 CC
  //

  AliInfo(" Settings for D*+ --> D0pi Pb-Pb collisions at 2.76 TeV - 0-7.5 centrality - DUMMY"); 


 // Normalization
  fNorm = new TH1F("fNorm","fNorm",36,0,36);
  for(Int_t i=1;i<=36;i++) fNorm->SetBinContent(i,0.048); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",36,0,36);
  for(Int_t i=1;i<=36;i++) fBR->SetBinContent(i,0.015); // 1.5% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",36,0,36);;
  for(Int_t i=1;i<=36;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track


  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",36,0,36);
  for(Int_t i=1;i<=36;i++) fRawYield->SetBinContent(i,0.05);  //4%
  fRawYield->SetBinContent(4,0.2);
  fRawYield->SetBinContent(5,0.10);
  fRawYield->SetBinContent(6,0.10);
  fRawYield->SetBinContent(7,0.08);
  for(Int_t i=25;i<=36;i++) fRawYield->SetBinContent(i,0.15);  //4%

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",36,0,36);
  for(Int_t i=1;i<=36;i++) fCutsEff->SetBinContent(i,0.10); // 10%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",36,0,36);
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.05); // 3%
  fPIDEff->SetBinContent(4,0.09);

  // MC dN/dpt  (from study on D* pt shape)
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",36,0,36);
  for(Int_t i=1;i<=36;i++) fMCPtShape->SetBinContent(i,0.035);
  fMCPtShape->SetBinContent(4,0.015);
  fMCPtShape->SetBinContent(5,0.015);
  fMCPtShape->SetBinContent(6,0.015);
  fMCPtShape->SetBinContent(7,0.02);
  fMCPtShape->SetBinContent(8,0.02);
  fMCPtShape->SetBinContent(9,0.03);
  fMCPtShape->SetBinContent(10,0.03);
  fMCPtShape->SetBinContent(11,0.03);
  fMCPtShape->SetBinContent(12,0.03);
  
  

  return;

}
//-------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010PbPb2040() {
  // 
  // D*+->D0pi syst errors. Responsible: A. Grelli
  //  2010 PbPb sample, 20-40 CC
  //

  AliInfo(" Settings for D*+ --> D0pi Pb-Pb collisions at 2.76 TeV - 20-40 centrality - DUMMY"); 

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.10); // 10% error on sigmaV0and

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",24,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.015); // 1.5% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);;
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track


  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  for(Int_t i=1;i<=24;i++) fRawYield->SetBinContent(i,0.15);  //4%
 
  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fCutsEff->SetBinContent(i,0.10); // 10%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.04); // 3%
 

  // MC dN/dpt  (copied from D0 : will update later)
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0.);
  fMCPtShape->SetBinContent(1,0.03);
  fMCPtShape->SetBinContent(2,0.03);

  return;

}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010PbPb4080() {
  // 
  // D*+->D0pi syst errors. Responsible: A. Grelli
  //  2010 PbPb sample, 40-80 CC
  //

  AliInfo(" Settings for D*+ --> D0pi Pb-Pb collisions at 2.76 TeV - 40-80 centrality - DUMMY"); 

 // Normalization
  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.07); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",24,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.015); // 1.5% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);;
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track


  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  for(Int_t i=1;i<=24;i++) fRawYield->SetBinContent(i,0.2);  //4%
  fRawYield->SetBinContent(1,1);
  fRawYield->SetBinContent(2,0.15);
  fRawYield->SetBinContent(3,0.15);
  fRawYield->SetBinContent(4,0.15);
  fRawYield->SetBinContent(5,0.15);
  fRawYield->SetBinContent(6,0.10);
  fRawYield->SetBinContent(7,0.10);
  fRawYield->SetBinContent(8,0.10);
  fRawYield->SetBinContent(9,0.11);
  fRawYield->SetBinContent(10,0.11);
  fRawYield->SetBinContent(11,0.11);
  fRawYield->SetBinContent(12,0.11);
  fRawYield->SetBinContent(13,0.08);
  fRawYield->SetBinContent(14,0.08);
  fRawYield->SetBinContent(15,0.08);
  fRawYield->SetBinContent(16,0.08);


  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fCutsEff->SetBinContent(i,0.10); // 10%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.05); // 3%
 

  // MC dN/dpt  (copied from D0 : will update later)
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  fMCPtShape->SetBinContent(2,0.05);
  fMCPtShape->SetBinContent(3,0.05);
  fMCPtShape->SetBinContent(4,0.05);
  fMCPtShape->SetBinContent(5,0.04);
  fMCPtShape->SetBinContent(6,0.02);
  fMCPtShape->SetBinContent(7,0.04);
  fMCPtShape->SetBinContent(8,0.04);
 
  return;

}

// ----------------------------- 2011 ----------------------------------
void AliHFSystErr::InitDstartoD0pi2011PbPb010() {
  // 
  // D*+->D0pi syst errors. Responsible: A. Grelli
  //  2011 PbPb sample, 0-10% CC
  //

  AliInfo(" Settings for D*+ --> D0pi Pb-Pb collisions at 2.76 TeV - 0-10% centrality "); 


 // Normalization
  fNorm = new TH1F("fNorm","fNorm",36,0,36);
  for(Int_t i=1;i<=36;i++) fNorm->SetBinContent(i,0.048); // TAA and pp norm

  // Branching ratio 
  fBR = new TH1F("fBR","fBR",36,0,36);
  for(Int_t i=1;i<=36;i++) fBR->SetBinContent(i,0.015); // 1.5% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",36,0,36);;
  for(Int_t i=1;i<=36;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track


  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",36,0,36);
  for(Int_t i=1;i<=3;i++) fRawYield->SetBinContent(i, 0.);
  fRawYield->SetBinContent(4,0.10);
  fRawYield->SetBinContent(5,0.08);
  fRawYield->SetBinContent(6,0.05);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=25;i<=36;i++) fRawYield->SetBinContent(i,0.10);  //4%

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",36,0,36);
  for(Int_t i=1;i<=3;i++) fCutsEff->SetBinContent(i, 0.);
  fCutsEff->SetBinContent(4,0.15);
  for(Int_t i=5;i<=36;i++) fCutsEff->SetBinContent(i,0.10); // 10%

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",36,0,36);
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.05); // 3%

  // MC dN/dpt  (from study on D* pt shape)
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",36,0,36);
  for(Int_t i=1;i<=3;i++) fMCPtShape->SetBinContent(i, 0.);
  fMCPtShape->SetBinContent(4,0.06);
  fMCPtShape->SetBinContent(5,0.03);
  for(Int_t i=5;i<=36;i++) fMCPtShape->SetBinContent(i,0.01);
 
  return;

}

//----------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2011PbPb3050() {
    //
    // D*+->D0pi syst errors. Responsible: A. Grelli
    //  2011 PbPb sample, 30-50 CC
    //
    
    AliInfo(" Settings for D*+ --> D0pi Pb-Pb collisions at 2.76 TeV - 30-50 centrality - DUMMY");
    
    // Normalization
    fNorm = new TH1F("fNorm","fNorm",24,0,24);
    for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.07); // TAA and pp norm
    
    // Branching ratio
    fBR = new TH1F("fBR","fBR",24,0,24);
    for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.015); // 1.5% PDG2010
    
    // Tracking efficiency
    fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);;
    for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.15); // Jacek, 5% per track
    
    
    // Raw yield extraction
    fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
    for(Int_t i=1;i<=24;i++) fRawYield->SetBinContent(i,0.1);  //4%
    fRawYield->SetBinContent(1,0.12);
    fRawYield->SetBinContent(2,0.12);
    fRawYield->SetBinContent(3,0.12);
    fRawYield->SetBinContent(4,0.06);
    fRawYield->SetBinContent(5,0.06);
    fRawYield->SetBinContent(6,0.05);
    fRawYield->SetBinContent(7,0.08);
    fRawYield->SetBinContent(8,0.05);
    fRawYield->SetBinContent(9,0.05);
    fRawYield->SetBinContent(10,0.05);
    fRawYield->SetBinContent(11,0.05);
    fRawYield->SetBinContent(12,0.05);
    fRawYield->SetBinContent(13,0.05);
    fRawYield->SetBinContent(14,0.05);
    fRawYield->SetBinContent(15,0.05);
    fRawYield->SetBinContent(16,0.05);
    
    
    // Cuts efficiency (from cuts variation)
    fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
    for(Int_t i=1;i<=24;i++) fCutsEff->SetBinContent(i,0.05); // 10%
    fCutsEff->SetBinContent(1,0.16);
    fCutsEff->SetBinContent(2,0.16);
    fCutsEff->SetBinContent(3,0.16);
    fCutsEff->SetBinContent(4,0.10);
    fCutsEff->SetBinContent(5,0.10);
    fCutsEff->SetBinContent(6,0.10);
    // PID efficiency (from PID/noPID)
    fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
    for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.05); // 3%
    
    
    // MC dN/dpt  (copied from D0 : will update later)
    fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
    for(Int_t i=1;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
    fMCPtShape->SetBinContent(2,0.11);
    fMCPtShape->SetBinContent(3,0.11);
    fMCPtShape->SetBinContent(4,0.03);
    fMCPtShape->SetBinContent(5,0.02);
    fMCPtShape->SetBinContent(6,0.01);
    fMCPtShape->SetBinContent(7,0.01);
    fMCPtShape->SetBinContent(8,0.01);
    
    return;
}
//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb0100(){
  // Dstar syst in pPb 2013 MB
  //
  AliInfo(" Settings for D* --> D0 pi, p-Pb collisions at 5.023 TeV"); 

  //Normalization
  fNorm = new TH1F("fNorm","fNorm",24,0,24);
  for(Int_t i=1;i<=24;i++) fNorm->SetBinContent(i,0.07); //
  
  // Branching ratio
  fBR = new TH1F("fBR","fBR",24,0,24);
  for(Int_t i=1;i<=24;i++) fBR->SetBinContent(i,0.015); // 1.5% PDG2010

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fTrackingEff->SetBinContent(i,0.09); // 3% per track

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,1.0);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.02);
  fRawYield->SetBinContent(4,0.02);
  fRawYield->SetBinContent(5,0.05);
  fRawYield->SetBinContent(6,0.02);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.05); 
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.06);

  // Cuts efficiency (from cuts variation)
  fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);

  // MC dN/dpt  
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",24,0,24);
  for(Int_t i=1;i<=3;i++) fMCPtShape->SetBinContent(i,0.03);
  for(Int_t i=4;i<=6;i++) fMCPtShape->SetBinContent(i,0.);
  for(Int_t i=7;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);

  // particle-antiparticle
  /*
  fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
  fPartAntipart->SetBinContent(1,1);
  fPartAntipart->SetBinContent(2,1);
  fPartAntipart->SetBinContent(3,0.12);
  for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
  */
  
    return;

}

//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010PbPb010CentScan(){
  // define errors for RAA vs. centrality
  InitD0toKpi2010PbPb020();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=3;i<=5;i++) fMCPtShape->SetBinContent(i,0.17);      
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.08); 
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010PbPb1020CentScan(){
  // define errors for RAA vs. centrality
  InitD0toKpi2010PbPb020();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=3;i<=5;i++)  fMCPtShape->SetBinContent(i,0.17);    
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.08); 
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010PbPb2040CentScan(){
  // define errors for RAA vs. centrality
  InitD0toKpi2010PbPb4080();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=3;i<=5;i++)  fMCPtShape->SetBinContent(i,0.14);
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.08); 
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010PbPb4060CentScan(){
   // define errors for RAA vs. centrality
  InitD0toKpi2010PbPb4080();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=3;i<=5;i++)  fMCPtShape->SetBinContent(i,0.11);  
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.08); 
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010PbPb6080CentScan(){
   // define errors for RAA vs. centrality
  InitD0toKpi2010PbPb4080();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=3;i<=5;i++) fMCPtShape->SetBinContent(i,0.08);
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.08); 
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2010PbPb010CentScan(){
  // define errors for RAA vs. centrality
  InitDplustoKpipi2010PbPb020();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.18);
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.09);

}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2010PbPb1020CentScan(){
  // define errors for RAA vs. centrality
  InitDplustoKpipi2010PbPb020();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.23);
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.08);
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2010PbPb2040CentScan(){
  // define errors for RAA vs. centrality
  InitDplustoKpipi2010PbPb020();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.095);
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2010PbPb4060CentScan(){
  // define errors for RAA vs. centrality
  InitDplustoKpipi2010PbPb4080();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.08);
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2010PbPb6080CentScan(){
  // define errors for RAA vs. centrality
  InitDplustoKpipi2010PbPb4080();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.15);
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.07);
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010PbPb010CentScan(){
  // define errors for RAA vs. centrality
  InitDstartoD0pi2010PbPb020();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.16); 
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.15);
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010PbPb1020CentScan(){
  // define errors for RAA vs. centrality
  InitDstartoD0pi2010PbPb020();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.05); 
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.15);
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010PbPb2040CentScan(){
  // define errors for RAA vs. centrality
  InitDstartoD0pi2010PbPb2040();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.10); 
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.08);
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010PbPb4060CentScan(){
  // define errors for RAA vs. centrality
  InitDstartoD0pi2010PbPb4080();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.10); 
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.045);
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010PbPb6080CentScan(){
  // define errors for RAA vs. centrality
  InitDstartoD0pi2010PbPb4080();
  for(Int_t i=7;i<=12;i++) fRawYield->SetBinContent(i,0.10); 
  for(Int_t i=7;i<=12;i++) fMCPtShape->SetBinContent(i,0.045);
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2011PbPb010CentScan(){
  // define errors for RAA vs. centrality 2011
  InitD0toKpi2011PbPb07half();
  
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2011PbPb1020CentScan(){
  // define errors for RAA vs. centrality 2011
  InitD0toKpi2011PbPb07half();
  
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2011PbPb2030CentScan(){
  // define errors for RAA vs. centrality 2011
  InitD0toKpi2011PbPb07half();
  
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2011PbPb3040CentScan(){
  // define errors for RAA vs. centrality 2011
  InitD0toKpi2011PbPb07half();
  
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2011PbPb4050CentScan(){
  // define errors for RAA vs. centrality 2011
  InitD0toKpi2011PbPb07half();
  
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitD0toKpi2010PbPb5080CentScan(){
  // define errors for RAA vs. centrality 2011
  InitD0toKpi2011PbPb07half();
  
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2011PbPb010CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDplustoKpipi2011PbPb07half();

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.15);
  for(Int_t i=5;i<=8;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9;i<=17;i++) fRawYield->SetBinContent(i,0.15);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=5;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=17;i++) fMCPtShape->SetBinContent(i,0.05);

  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.15);
  for(Int_t i=5;i<=8;i++) fCutsEff->SetBinContent(i,0.20);
  for(Int_t i=9;i<=17;i++) fCutsEff->SetBinContent(i,0.10);
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2011PbPb1020CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDplustoKpipi2011PbPb07half();

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=5;i<=8;i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=9;i<=17;i++)fRawYield->SetBinContent(i,0.15);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=5;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=17;i++) fMCPtShape->SetBinContent(i,0.05);
  
  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.15);
  for(Int_t i=5;i<=8;i++) fCutsEff->SetBinContent(i,0.20);
  for(Int_t i=9;i<=17;i++) fCutsEff->SetBinContent(i,0.15);
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2011PbPb2030CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDplustoKpipi2011PbPb07half();
  
  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=5;i<=8;i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=9;i<=17;i++) fRawYield->SetBinContent(i,0.08);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=5;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=17;i++) fMCPtShape->SetBinContent(i,0.05);
  
  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.20);
  for(Int_t i=5;i<=8;i++) fCutsEff->SetBinContent(i,0.15);
  for(Int_t i=9;i<=17;i++) fCutsEff->SetBinContent(i,0.20);
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2011PbPb3040CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDplustoKpipi2011PbPb07half();

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.20);
  for(Int_t i=5;i<=8;i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=9;i<=17;i++) fRawYield->SetBinContent(i,0.20);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=5;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=17;i++) fMCPtShape->SetBinContent(i,0.05);

  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.20);
  for(Int_t i=5;i<=8;i++) fCutsEff->SetBinContent(i,0.20);
  for(Int_t i=9;i<=17;i++) fCutsEff->SetBinContent(i,0.20);
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2011PbPb4050CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDplustoKpipi2011PbPb07half();

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=5;i<=8;i++) fRawYield->SetBinContent(i,0.10);
  for(Int_t i=9;i<=17;i++) fRawYield->SetBinContent(i,0.05);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=5;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=17;i++) fMCPtShape->SetBinContent(i,0.05);

  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.20);
  for(Int_t i=5;i<=8;i++) fCutsEff->SetBinContent(i,0.15);
  for(Int_t i=9;i<=17;i++) fCutsEff->SetBinContent(i,0.20);
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDplustoKpipi2010PbPb5080CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDplustoKpipi2010PbPb4080();

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=5;i<=8;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9;i<=17;i++) fRawYield->SetBinContent(i,0.10);

  for(Int_t i=1;i<=17;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.20); // 10%
  for(Int_t i=5;i<=8;i++) fCutsEff->SetBinContent(i,0.10); // 10%
  for(Int_t i=9;i<=17;i++) fCutsEff->SetBinContent(i,0.20); // 10%
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2011PbPb010CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDstartoD0pi2011PbPb07half();
  
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2011PbPb1020CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDstartoD0pi2011PbPb07half();
  
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2011PbPb2030CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDstartoD0pi2011PbPb07half();
  
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2011PbPb3040CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDstartoD0pi2011PbPb07half();
  
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2011PbPb4050CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDstartoD0pi2011PbPb07half();
  
}
//--------------------------------------------------------------------------
void AliHFSystErr::InitDstartoD0pi2010PbPb5080CentScan(){
  // define errors for RAA vs. centrality 2011
  InitDstartoD0pi2011PbPb07half();
  
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb0100RapScan0804(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, rapidity scan -0.8 < y < -0.4");

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.04);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.04);
  
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%
 
  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.05);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);
  

};
//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb0100RapScan0401(){
  InitD0toKpi2013pPb0100();

   AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, rapidity scan -0.4 < y < -0.1");
  
  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.04);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.04);
  
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%

   for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.05);
   for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.05);
   for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.05);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);
};
//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb0100RapScan0101(){
  InitD0toKpi2013pPb0100();

  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, rapidity scan -0.1 < y < 0.1");

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.04);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.12);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.08);

  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%

  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.05);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);

};
//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb0100RapScan0104(){
  InitD0toKpi2013pPb0100();

  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, rapidity scan 0.1 < y < 0.4");

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.04);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.03);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%
  
  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);

};
//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb0100RapScan0408(){
  InitD0toKpi2013pPb0100();

  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, rapidity scan 0.4 < y < 0.8");

   for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.04);
   for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.03);
   for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.04);

  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%
  
  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.05);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);
  
};

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb0100RapScan0804(){
  InitDplustoKpipi2013pPb0100();

  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, rapidity scan -0.8 < y < -0.4");

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.07);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.10);
  
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%
  
  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.07);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.08);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.08);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.10);

};
//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb0100RapScan0401(){
  InitDplustoKpipi2013pPb0100();
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, rapidity scan -0.4 < y < -0.1");
  
   for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.10);

  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%

  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.07);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.08);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.08);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);

};
//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb0100RapScan0101(){
  InitDplustoKpipi2013pPb0100();
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, rapidity scan -0.1 < y < 0.1");

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.05);

  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%

  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.07);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.08);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.08);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);

};
//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb0100RapScan0104(){
  InitDplustoKpipi2013pPb0100();
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, rapidity scan 0.1 < y < 0.4");
  
  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.05);
  
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%

  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.07);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.08);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.08);
  
  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);

};
//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb0100RapScan0408(){
  InitDplustoKpipi2013pPb0100();

  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, rapidity scan 0.4 < y < 0.8");

   for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.07);
   for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.05);
   for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.07);

  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%
  
  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.07);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.08);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.08);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);
  
};


//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb0100RapScan0804(){
  InitDstartoD0pi2013pPb0100();

  AliInfo("Settings for D*+ --> D0 pi, pPb collisions at 5.023 TeV, rapidity scan -0.8 < y < -0.4");

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.03);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.07);
  
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%

  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.05);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);

};
//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb0100RapScan0401(){
  InitDstartoD0pi2013pPb0100();

  AliInfo("Settings for D*+ --> D0 pi, pPb collisions at 5.023 TeV, rapidity scan -0.4 < y < -0.1");  

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.04);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.05);
  
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%

  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.05);

  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);

};
//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb0100RapScan0101(){
InitDstartoD0pi2013pPb0100();

   AliInfo("Settings for D*+ --> D0 pi, pPb collisions at 5.023 TeV, rapidity scan -0.1 < y < 0.1");

  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.03);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.05);

  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%

  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);

};
//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb0100RapScan0104(){
  InitDstartoD0pi2013pPb0100();

  AliInfo("Settings for D*+ --> D0 pi, pPb collisions at 5.023 TeV, rapidity scan 0.1 < y < 0.4");  
  
  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.04);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.03);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.05);
  
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%
  
  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);
};
//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb0100RapScan0408(){
  InitDstartoD0pi2013pPb0100();

  AliInfo("Settings for D*+ --> D0 pi, pPb collisions at 5.023 TeV, rapidity scan 0.4 < y < 0.8");  
  
  for(Int_t i=2;i<=5;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=6;i<=8;i++) fRawYield->SetBinContent(i,0.07);
  for(Int_t i=9;i<=16;i++) fRawYield->SetBinContent(i,0.07);
  
  for(Int_t i=1;i<=36;i++) fPIDEff->SetBinContent(i,0.0); // 0%
  
  for(Int_t i=2;i<=5;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=6;i<=8;i++) fCutsEff->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  for(Int_t i=2;i<=5;i++) fMCPtShape->SetBinContent(i,0.10);
  for(Int_t i=6;i<=8;i++) fMCPtShape->SetBinContent(i,0.05);
  for(Int_t i=9;i<=16;i++) fMCPtShape->SetBinContent(i,0.05);

};

///_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb020V0A(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 0-20% mult V0A");
  
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb2040V0A(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 20-40% mult V0A");
  
  
  // Raw yield extraction
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb4060V0A(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 40-60% mult V0A");
  
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb60100V0A(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 60-100% mult V0A");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.072);
  fMCPtShape->SetBinContent(3,0.06);
  fMCPtShape->SetBinContent(4,0.06);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.02);
  
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb020ZNA(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 0-20% mult ZNA");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.053);
  fMCPtShape->SetBinContent(3,0.05);
  fMCPtShape->SetBinContent(4,0.05);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb2040ZNA(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 20-40% mult ZNA");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.053);
  fMCPtShape->SetBinContent(3,0.05);
  fMCPtShape->SetBinContent(4,0.05);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb4060ZNA(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 40-60% mult ZNA");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.053);
  fMCPtShape->SetBinContent(3,0.05);
  fMCPtShape->SetBinContent(4,0.05);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb60100ZNA(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 60-100% mult ZNA");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.010);
  fMCPtShape->SetBinContent(3,0.010);
  fMCPtShape->SetBinContent(4,0.010);
  fMCPtShape->SetBinContent(5,0.05);
  fMCPtShape->SetBinContent(6,0.05);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb020CL1(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 0-20% mult CL1");
  
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb2040CL1(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 20-40% mult CL1");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb4060CL1(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 40-60% mult CL1");
  
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);}

//_________________________________________________________________________
void AliHFSystErr::InitD0toKpi2013pPb60100CL1(){
  InitD0toKpi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 60-100% mult CL1");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.085);
  for(Int_t i=3;i<=4;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=5;i<=6;i++) fRawYield->SetBinContent(i,0.042);
  for(Int_t i=7;i<=8;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.058);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.09);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.09);
  
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.072);
  fMCPtShape->SetBinContent(3,0.06);
  fMCPtShape->SetBinContent(4,0.06);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.02);
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb020V0A(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 0-20% mult V0A");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.10);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.08);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb2040V0A(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 20-40% mult V0A");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.10);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.00);
  
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb4060V0A(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 40-60% mult V0A");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.10);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.00);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb60100V0A(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 60-100% mult V0A");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.0);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.00);
  
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.072);
  fMCPtShape->SetBinContent(3,0.06);
  fMCPtShape->SetBinContent(4,0.06);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.02);
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb020ZNA(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 0-20% mult V0A");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.10);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.00);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  // fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.);
  
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.053);
  fMCPtShape->SetBinContent(3,0.05);
  fMCPtShape->SetBinContent(4,0.05);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb2040ZNA(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 20-40% mult V0A");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.10);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.08);
  
  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.053);
  fMCPtShape->SetBinContent(3,0.05);
  fMCPtShape->SetBinContent(4,0.05);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb4060ZNA(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D0 --> K pi, pPb collisions at 5.023 TeV, 40-60% mult V0A");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.10);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.00);
  
  
  // Cuts efficiency (from cuts variation)
  // fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  // fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.053);
  fMCPtShape->SetBinContent(3,0.05);
  fMCPtShape->SetBinContent(4,0.05);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
  
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb60100ZNA(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 60-100% mult V0A");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.00);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.00);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  // fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.010);
  fMCPtShape->SetBinContent(3,0.010);
  fMCPtShape->SetBinContent(4,0.010);
  fMCPtShape->SetBinContent(5,0.05);
  fMCPtShape->SetBinContent(6,0.05);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
}


//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb020CL1(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 0-20% mult CL1");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.10);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.08);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb2040CL1(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 20-40% mult CL1");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.10);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.00);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb4060CL1(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 40-60% mult CL1");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.10);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.0);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  // MC dN/dpt
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDstartoD0pi2013pPb60100CL1(){
  InitDstartoD0pi2013pPb0100();
  
  AliInfo("Settings for D* --> D0 pi, pPb collisions at 5.023 TeV, 60-100% mult CL1");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.00);
  fRawYield->SetBinContent(3,0.05);
  fRawYield->SetBinContent(4,0.05);
  fRawYield->SetBinContent(5,0.04);
  fRawYield->SetBinContent(6,0.04);
  fRawYield->SetBinContent(7,0.04);
  fRawYield->SetBinContent(8,0.04);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.00);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.0);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.072);
  fMCPtShape->SetBinContent(3,0.06);
  fMCPtShape->SetBinContent(4,0.06);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.02);
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb020V0A(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 0-20% mult V0A");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.08);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  //MC ptshape + multiplicty weights
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=4;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb2040V0A(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 20-40% mult V0A");
  
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.08);
  
  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  //MC ptshape + multiplicty weights
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=4;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb4060V0A(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 40-60% mult V0A");
  
  // Raw yield extraction
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.00);
  
  
  
  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  //MC ptshape + multiplicty weights
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb60100V0A(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 60-100% mult V0A");
  
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.08);
  
  
  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  //MC ptshape + multiplicty weights
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.072);
  fMCPtShape->SetBinContent(3,0.06);
  fMCPtShape->SetBinContent(4,0.06);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.02);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb020ZNA(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 0-20% mult ZNA");
  
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.08);
  
  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  // fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.053);
  fMCPtShape->SetBinContent(3,0.05);
  fMCPtShape->SetBinContent(4,0.05);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb2040ZNA(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 20-40% mult ZNA");
  
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.08);
  
  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  
  // PID efficiency (from PID/noPID)
  //  fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.053);
  fMCPtShape->SetBinContent(3,0.05);
  fMCPtShape->SetBinContent(4,0.05);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb4060ZNA(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 40-60% mult ZNA");
  
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.00);
  
  
  // Cuts efficiency (from cuts variation)
  // fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  
  // PID efficiency (from PID/noPID)
  // fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.053);
  fMCPtShape->SetBinContent(3,0.05);
  fMCPtShape->SetBinContent(4,0.05);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb60100ZNA(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 60-100% mult ZNA");
  
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.00);
  
  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  // fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.010);
  fMCPtShape->SetBinContent(3,0.010);
  fMCPtShape->SetBinContent(4,0.010);
  fMCPtShape->SetBinContent(5,0.05);
  fMCPtShape->SetBinContent(6,0.05);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.03);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
}


//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb020CL1(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 0-20% mult CL1");
  
  // Raw yield extraction
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.0);
  
  // Cuts efficiency (from cuts variation)
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  // PID efficiency (from PID/noPID)
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb2040CL1(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 20-40% mult CL1");
  
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.0);
  
  
  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb4060CL1(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 40-60% mult CL1");
  
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.08);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.0);
  
  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.10);
  for(Int_t i=3;i<=16;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.00);
  
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.028);
  fMCPtShape->SetBinContent(3,0.02);
  fMCPtShape->SetBinContent(4,0.02);
  for(Int_t i=5;i<=24;i++) fMCPtShape->SetBinContent(i,0.01);
  
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//_________________________________________________________________________
void AliHFSystErr::InitDplustoKpipi2013pPb60100CL1(){
  InitDplustoKpipi2013pPb0100();
  
  AliInfo("Settings for D+ --> K pi pi, pPb collisions at 5.023 TeV, 60-100% mult CL1");
  
  // Raw yield extraction
  //fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.0);
  fRawYield->SetBinContent(2,0.00);
  fRawYield->SetBinContent(3,0.03);
  fRawYield->SetBinContent(4,0.03);
  fRawYield->SetBinContent(5,0.03);
  fRawYield->SetBinContent(6,0.03);
  fRawYield->SetBinContent(7,0.06);
  fRawYield->SetBinContent(8,0.06);
  for(Int_t i=9;i<=12;i++) fRawYield->SetBinContent(i,0.06);
  for(Int_t i=13;i<=16;i++) fRawYield->SetBinContent(i,0.08);
  for(Int_t i=17;i<=24;i++) fRawYield->SetBinContent(i,0.08);
  
  
  // Cuts efficiency (from cuts variation)
  //fCutsEff = new TH1F("fCutsEff","fCutsEff",24,0,24);
  fCutsEff->SetBinContent(1,0.0);
  fCutsEff->SetBinContent(2,0.0);
  for(Int_t i=3;i<=24;i++) fCutsEff->SetBinContent(i,0.05);
  
  // PID efficiency (from PID/noPID)
  //fPIDEff = new TH1F("fPIDEff","fPIDEff",24,0,24);
  for(Int_t i=1;i<=24;i++) fPIDEff->SetBinContent(i,0.0);
  
  //MC ptshape + multiplicty weights
  fMCPtShape->SetBinContent(1,0.0);
  fMCPtShape->SetBinContent(2,0.072);
  fMCPtShape->SetBinContent(3,0.06);
  fMCPtShape->SetBinContent(4,0.06);
  fMCPtShape->SetBinContent(5,0.03);
  fMCPtShape->SetBinContent(6,0.03);
  for(Int_t i=7;i<=16;i++) fMCPtShape->SetBinContent(i,0.02);
  
  // particle-antiparticle
  /*
   fPartAntipart = new TH1F("fPartAntipart","fPartAntipart",20,0,20);
   fPartAntipart->SetBinContent(1,1);
   fPartAntipart->SetBinContent(2,1);
   fPartAntipart->SetBinContent(3,0.12);
   for(Int_t i=4;i<=20;i++) fPartAntipart->SetBinContent(i,0.05);   //5 to 12%
   */
  
  
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitLctopKpi2010pp() {
  //
  // Lc->pKpi syst errors. Responsible: R. Romita
  //  2010 pp sample
  //

  // Normalization
  fNorm = new TH1F("fNorm","fNorm",4,2,6);
  for(Int_t i=1;i<=4;i++) fNorm->SetBinContent(i,0.035); // 4% error on sigmaV0and

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",4,2,6);
  for(Int_t i=1;i<=4;i++) fTrackingEff->SetBinContent(i,0.12); // 12% (4% per track)

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",24,0,24);
  fRawYield->SetBinContent(1,0.20);
  fRawYield->SetBinContent(2,0.15);
  fRawYield->SetBinContent(3,0.10);
  fRawYield->SetBinContent(4,0.15);

  fCutsEff = new TH1F("fCutsEff","fCutsEff",4,2,6);
  fCutsEff->SetBinContent(1,0.50);
  fCutsEff->SetBinContent(2,0.07);
  fCutsEff->SetBinContent(3,0.30);
  fCutsEff->SetBinContent(4,0.40);

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",4,2,4);
  fPIDEff->SetBinContent(1,0.13); // 15%
  fPIDEff->SetBinContent(2,0.30); // 15%
  fPIDEff->SetBinContent(3,0.5); // 15%
  fPIDEff->SetBinContent(4,0.30); // 15%

  // MC dN/dpt 
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",12,0,12);
  for(Int_t i=1; i<=2; i++) fMCPtShape->SetBinContent(i,1.);
  for(Int_t i=3; i<=4; i++) fMCPtShape->SetBinContent(i,0.03);
  for(Int_t i=5; i<=6; i++) fMCPtShape->SetBinContent(i,0.03);
  for(Int_t i=7; i<=8; i++) fMCPtShape->SetBinContent(i,0.02);
  for(Int_t i=9; i<=12; i++) fMCPtShape->SetBinContent(i,0.02);
}

//--------------------------------------------------------------------------
void AliHFSystErr::InitLctopK0S2010pp() {
  //
  // Lc->pK0S syst errors. Responsible: A. De Caro
  //  2010 pp sample
  //  Only tool preparation: no physical mean for these values!
  //

  // Normalization
  fNorm = new TH1F("fNorm","fNorm",12,0.,12.);
  for(Int_t i=1;i<=12;i++) fNorm->SetBinContent(i,0.05); // 5% error on sigmaV0and // only a try

  // Tracking efficiency
  fTrackingEff = new TH1F("fTrackingEff","fTrackingEff",12,0.,12.);
  for(Int_t i=1;i<=12;i++) fTrackingEff->SetBinContent(i,0.1); // 10% (4% per track) // only a try

  // Raw yield extraction
  fRawYield = new TH1F("fRawYield","fRawYield",12,0.,12.);
  for(Int_t i=1;i<=2;i++) fRawYield->SetBinContent(i,1.); // only a try
  for(Int_t i=3;i<=12;i++) fRawYield->SetBinContent(i,0.1); // only a try

  fCutsEff = new TH1F("fCutsEff","fCutsEff",12,0.,12.);
  for(Int_t i=1;i<=12;i++) fCutsEff->SetBinContent(i,0.1); // only a try

  // PID efficiency (from PID/noPID)
  fPIDEff = new TH1F("fPIDEff","fPIDEff",12,0.,12.);
  for(Int_t i=1;i<=12;i++) fPIDEff->SetBinContent(i,0.1); // only a try

  // MC dN/dpt 
  fMCPtShape = new TH1F("fMCPtShape","fMCPtShape",12,0.,12.);
  for(Int_t i=1; i<=2;i++) fMCPtShape->SetBinContent(i,1.); // only a try
  for(Int_t i=3; i<=12;i++) fMCPtShape->SetBinContent(i,0.1); // only a try

}

//--------------------------------------------------------------------------
Double_t AliHFSystErr::GetCutsEffErr(Double_t pt) const {
  // 
  // Get error
  //

  Int_t bin=fCutsEff->FindBin(pt);

  return fCutsEff->GetBinContent(bin);
}
//--------------------------------------------------------------------------
Double_t AliHFSystErr::GetMCPtShapeErr(Double_t pt) const {
  // 
  // Get error
  //

  Int_t bin=fMCPtShape->FindBin(pt);

  return fMCPtShape->GetBinContent(bin);
}
//--------------------------------------------------------------------------
Double_t AliHFSystErr::GetSeleEffErr(Double_t pt) const {
  // 
  // Get error
  //

  Double_t err=GetCutsEffErr(pt)*GetCutsEffErr(pt)+GetMCPtShapeErr(pt)*GetMCPtShapeErr(pt);

  return TMath::Sqrt(err);
}
//--------------------------------------------------------------------------
Double_t AliHFSystErr::GetPIDEffErr(Double_t pt) const {
  // 
  // Get error
  //

  Int_t bin=fPIDEff->FindBin(pt);

  return fPIDEff->GetBinContent(bin);
}
//--------------------------------------------------------------------------
Double_t AliHFSystErr::GetTrackingEffErr(Double_t pt) const {
  // 
  // Get error
  //

  Int_t bin=fTrackingEff->FindBin(pt);

  return fTrackingEff->GetBinContent(bin);
}
//--------------------------------------------------------------------------
Double_t AliHFSystErr::GetRawYieldErr(Double_t pt) const {
  // 
  // Get error
  //

  Int_t bin=fRawYield->FindBin(pt);

  return fRawYield->GetBinContent(bin);
}
//--------------------------------------------------------------------------
Double_t AliHFSystErr::GetPartAntipartErr(Double_t pt) const {
  // 
  // Get error
  //

  Int_t bin=fPartAntipart->FindBin(pt);

  return fPartAntipart->GetBinContent(bin);
}
//--------------------------------------------------------------------------
Double_t AliHFSystErr::GetTotalSystErr(Double_t pt,Double_t feeddownErr) const {
  // 
  // Get total syst error (except norm. error)
  //

  Double_t err=0.;

  if(fRawYield) err += GetRawYieldErr(pt)*GetRawYieldErr(pt);
  if(fTrackingEff) err += GetTrackingEffErr(pt)*GetTrackingEffErr(pt);
  //  if(fBR) err += GetBRErr()*GetBRErr();
  if(fCutsEff) err += GetCutsEffErr(pt)*GetCutsEffErr(pt);
  if(fPIDEff) err += GetPIDEffErr(pt)*GetPIDEffErr(pt);
  if(fMCPtShape) err += GetMCPtShapeErr(pt)*GetMCPtShapeErr(pt);
  if(fPartAntipart) err += GetPartAntipartErr(pt)*GetPartAntipartErr(pt);

  err += feeddownErr*feeddownErr;

  return TMath::Sqrt(err);
}
//---------------------------------------------------------------------------
void AliHFSystErr::DrawErrors(TGraphAsymmErrors *grErrFeeddown) const {
  //
  // Draw errors
  //
  gStyle->SetOptStat(0);

  TCanvas *cSystErr = new TCanvas("cSystErr","Systematic Errors",300,80,640,500);
  cSystErr->Range(0.20,-0.5,18.4,0.34);
  cSystErr->SetRightMargin(0.318);
  cSystErr->SetFillColor(0);

  TH2F *hFrame = new TH2F("hFrame","Systematic errors; p_{t} (GeV/c); Relative Error",40,0,40,100,-1,+1);
  hFrame->SetAxisRange(1.,35.9,"X");
  if(fIsLowPtAnalysis) hFrame->SetAxisRange(0.,24.,"X");
  hFrame->SetAxisRange(-0.5,0.5,"Y");
  hFrame->Draw();

  TLegend *leg = new TLegend(0.69,0.44,0.98,0.86,NULL,"brNDC");
  leg->SetTextSize(0.03601695);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  
  TH1F *hTotErr=new TH1F("hTotErr","",36,0,36);
  Int_t nbins = fNorm->GetNbinsX();
  TGraphAsymmErrors *gTotErr = new TGraphAsymmErrors(nbins);
  for(Int_t i=1;i<=36;i++) {
    Double_t pt = hTotErr->GetBinCenter(i);
    Double_t ptwidth = hTotErr->GetBinWidth(i);

    if(grErrFeeddown) {
      Double_t x=0., y=0., errxl=0., errxh=0., erryl=0., erryh=0.;
      Double_t toterryl=0., toterryh=0.;
      for(Int_t j=0; j<grErrFeeddown->GetN(); j++) {
	grErrFeeddown->GetPoint(j,x,y);
	errxh = grErrFeeddown->GetErrorXhigh(j);
	errxl = grErrFeeddown->GetErrorXlow(j);
	if ( ( (x-errxl) <= pt) && ( (x+errxl) >= pt) ) {
	  erryh = grErrFeeddown->GetErrorYhigh(j);
	  erryl = grErrFeeddown->GetErrorYlow(j);
	}
      }
      if (erryl>=1e-3) toterryl = GetTotalSystErr(pt,erryl);
      else toterryl = GetTotalSystErr(pt);
      if (erryh>=1e-3) toterryh = GetTotalSystErr(pt,erryh);
      else toterryh = GetTotalSystErr(pt);

      hTotErr->SetBinContent(i,toterryh);
      gTotErr->SetPoint(i,pt,0.);
      gTotErr->SetPointError(i,ptwidth/2.,ptwidth/2.,toterryl,toterryh); // i, exl, exh, eyl, eyh
    }
    else {
      hTotErr->SetBinContent(i,GetTotalSystErr(pt));
      gTotErr->SetPoint(i,pt,0.);
      gTotErr->SetPointError(i,ptwidth/2.,ptwidth/2.,GetTotalSystErr(pt),GetTotalSystErr(pt)); // i, exl, exh, eyl, eyh
    }

  }
  gTotErr->SetLineColor(kBlack);
  gTotErr->SetFillColor(kRed);
  gTotErr->SetFillStyle(3002);
  gTotErr->Draw("2");
  leg->AddEntry(gTotErr,"Total (excl. norm.)","f");
//   hTotErr->SetLineColor(1);
//   hTotErr->SetLineWidth(3);
//   hTotErr->Draw("same");
//   leg->AddEntry(hTotErr,"Total (excl. norm.)","l");
  

  fNorm->SetFillColor(1);
  fNorm->SetFillStyle(3002);
  //fNorm->Draw("same");
  //TH1F *hNormRefl = ReflectHisto(fNorm);
  //hNormRefl->Draw("same");
  Double_t norm = fNorm->GetBinContent(1)*100;
  leg->AddEntry(fNorm,Form("Normalization (%.1f%s)",norm,"%"),"");

  if(grErrFeeddown) {
    grErrFeeddown->SetFillColor(kTeal-8);
    grErrFeeddown->SetFillStyle(3001);
    grErrFeeddown->Draw("2");
    leg->AddEntry(grErrFeeddown,"Feed-down from B","f");
  }
  if(fTrackingEff) {
    fTrackingEff->SetFillColor(4);
    fTrackingEff->SetFillStyle(3006);
    fTrackingEff->Draw("same");
    TH1F *hTrackingEffRefl = ReflectHisto(fTrackingEff);
    hTrackingEffRefl->Draw("same");
    leg->AddEntry(fTrackingEff,"Tracking efficiency","f");
  }
  if(fBR) {
    fBR->SetFillColor(6);
    fBR->SetFillStyle(3005);
    //fBR->SetFillStyle(3020);
    fBR->Draw("same");
    TH1F *hBRRefl = ReflectHisto(fBR);
    hBRRefl->Draw("same");
    leg->AddEntry(fBR,"Branching ratio","f");
  }
  if(fRawYield) {
    Int_t ci;   // for color index setting
    ci = TColor::GetColor("#00cc00");
    fRawYield->SetLineColor(ci);
    //    fRawYield->SetLineColor(3);
    fRawYield->SetLineWidth(3);
    fRawYield->Draw("same");
    TH1F *hRawYieldRefl = ReflectHisto(fRawYield);
    hRawYieldRefl->Draw("same");
    leg->AddEntry(fRawYield,"Yield extraction","l");
  }
  if(fCutsEff) {
    fCutsEff->SetLineColor(4);
    fCutsEff->SetLineWidth(3);
    fCutsEff->Draw("same");
    TH1F *hCutsEffRefl = ReflectHisto(fCutsEff);
    hCutsEffRefl->Draw("same");
    leg->AddEntry(fCutsEff,"Cuts efficiency","l");
  }
  if(fPIDEff) {
    fPIDEff->SetLineColor(7);
    fPIDEff->SetLineWidth(3);
    fPIDEff->Draw("same");
    TH1F *hPIDEffRefl = ReflectHisto(fPIDEff);
    hPIDEffRefl->Draw("same");
    leg->AddEntry(fPIDEff,"PID efficiency","l");
  }
  if(fMCPtShape) {
    Int_t ci = TColor::GetColor("#9933ff");
    fMCPtShape->SetLineColor(ci);
    //    fMCPtShape->SetLineColor(8);
    fMCPtShape->SetLineWidth(3);
    fMCPtShape->Draw("same");
    TH1F *hMCPtShapeRefl = ReflectHisto(fMCPtShape);
    hMCPtShapeRefl->Draw("same");
    leg->AddEntry(fMCPtShape,"MC p_{t} shape","l");
  }
  if(fPartAntipart) {
    Int_t ci = TColor::GetColor("#ff6600");
    fPartAntipart->SetLineColor(ci);
    //    fPartAntipart->SetLineColor(9);
    fPartAntipart->SetLineWidth(3);
    fPartAntipart->Draw("same");
    TH1F *hPartAntipartRefl = ReflectHisto(fPartAntipart);
    hPartAntipartRefl->Draw("same");
    leg->AddEntry(fPartAntipart,"D = #bar{D}","l");
  }


  leg->Draw();

  cSystErr->SaveAs("RelativeSystematics.eps");

  return;
}
//-------------------------------------------------------------------------
TH1F* AliHFSystErr::ReflectHisto(TH1F *hin) const {
  //
  // Clones and reflects histogram 
  // 
  TH1F *hout=(TH1F*)hin->Clone("hout");
  hout->Scale(-1.);

  return hout;
}



