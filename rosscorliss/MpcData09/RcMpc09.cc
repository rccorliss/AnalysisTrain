#include "Fun4AllServer.h"
#include "getClass.h"
#include "PHCompositeNode.h"
#include "Fun4AllHistoManager.h"
#include "Fun4AllReturnCodes.h"
#include "TriggerHelper.h"


#include "RcMpc09.hh"
#include "mpcRawContainer.h"
#include "mpcRawContent.h"
#include "mpcClusterContainer.h"
#include "mpcClusterContent.h"
#include "mpcTowerContainer.h"
#include "mpcTowerContent.h"
#include "recoConsts.h"
#include <PHGlobal.h>
#include "MpcMap.h"
#include "MpcCalib.h"
#include "TCanvas.h"
#include <TH1.h>
#include <TH1F.h>
#include <TH2.h>
#include <TTree.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TFile.h>

#include "TrigLvl1.h"
#include <TStyle.h>

#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;
using namespace findNode;

int RcMpc09::InitRun(PHCompositeNode *topNode)
{
  recoConsts *rc = recoConsts::instance();
  run = rc->get_IntFlag("RUNNUMBER");
  return EVENT_OK;
}

RcMpc09::RcMpc09(char *_foutfile) : SubsysReco("RcMpc09")
{
  outfile = _foutfile;
  run = 0;
  ievent = 0;

  //load calibration settings 
  mpccalib = MpcCalib::instance();
  mpcmap = MpcMap::instance();

  //set up our output tree:
  file = new TFile(outfile,"RECREATE");
  T = new TTree("T","T");

  T->Branch("event",&event,"event/I");
  T->Branch("nclus",&nclus,"nclus/I");
  //T->Branch("ntow",&ntow,"ntow/I");
  T->Branch("bbcvtx",&bbcvtx,"bbcvtx/F");
  T->Branch("zdcvtx",&zdcvtx,"zdcvtx/F");
  T->Branch("bunch",&bunch,"bunch/S");
  T->Branch("trig",&trig,"trig/S");

  T->Branch("ecore",ecore,"ecore[nclus]/F");
  T->Branch("feecore",feecore,"feecore[nclus]/S");
  T->Branch("pt",pt,"pt[nclus]/F");

  T->Branch("x",x,"x[nclus]/F");
  T->Branch("y",y,"y[nclus]/F");
  T->Branch("z",z,"z[nclus]/F");

  //cluster features:  mult (ntowers in cluster), disp (dispersion), chi2core (photon/neutron fit goodness in central tower)
  T->Branch("mult",mult,"mult[nclus]/S");
  T->Branch("disp",disp,"disp[nclus]/F");
  T->Branch("chi2core",chi2core,"chi2core[nclus]/F");

  T->Branch("tdc_max",tdc_max,"tdc_max[nclus]/S");
  T->Branch("lg_post_core",lg_post_core,"lg_post_core[nclus]/S");

  T->Branch("e8e9", &e8e9, "e8e9[nclus]/F");
  T->Branch("cent_ch", &cent_ch, "cent_ch[nclus]/I");
  
  //T->Branch("etow",etow,"etow[ntow]/F");
  //T->Branch("chtow",chtow,"chtow[ntow]/S");

  // Other annoying variables that we are suppossed to initialize in the
  //constructor

  ntow = 0;
  event = 0;
  nclus = 0;
  ntow = 0;
  bbcvtx = 0;
  zdcvtx = 0;
  bunch = 0;
  trig = 0;
  for (int iclus=0; iclus<MAXCLUSTERS; iclus++)
    {
      ecore[iclus]=0;
      feecore[iclus]=0;
      pt[iclus]=0;
      x[iclus]=0;
      y[iclus]=0;
      z[iclus]=0;
      mult[iclus]=0;
      dispx[iclus]=0;
      dispy[iclus]=0;
      disp[iclus]=0;
      e8e9[iclus]=0;
      cent_ch[iclus]=0;
      chi2core[iclus]=0;
      tdc_max[iclus] = 0;
      lg_post_core[iclus] = 0;
    }

  for (int itow = 0; itow < MAXTOWERS; itow++)
    {
      chtow[itow] = -1;
      etow[itow]=-1;
    }

  for (int itow = 0; itow < MAXTOWERS; itow++)
    {
      chtow[itow] = -1;
      etow[itow]=-1;
    }

  // phglobal = new PHGlobal;
  // mpcclus = new mpcClusterContainer;
  // //mpcraw = new mpcRawContainer;
  // mpctow = new mpcTowerContainer;
  // triglvl1 = new TrigLvl1;
  phglobal = NULL;
  mpcclus = NULL;
  //mpcraw = new mpcRawContainer;
  mpctow = NULL;
  triglvl1 = NULL;

}

int RcMpc09::Init(PHCompositeNode *topNode)
{
  return 0;
}

int RcMpc09::End(PHCompositeNode *topNode)
{
  file->Write();
  delete file;
  return 0;
}

int RcMpc09::ResetVars()
{
  event = 0;
  nclus = 0;
  ntow = 0;
  bbcvtx = 0;
  zdcvtx = 0;
  bunch = 0;
  trig = 0;

  for (int iclus=0; iclus<MAXCLUSTERS; iclus++)
    {
      ecore[iclus]=0;
      feecore[iclus]=0;
      pt[iclus]=0;
      x[iclus]=0;
      y[iclus]=0;
      z[iclus]=0;

      mult[iclus]=0;
      dispx[iclus]=0;
      dispy[iclus]=0;
      disp[iclus]=0;
      e8e9[iclus]=0;
      cent_ch[iclus]=0;
      chi2core[iclus]=0;


      tdc_max[iclus] = 0;
      //lg_adc_core[iclus] = 0;
      //hg_adc_core[iclus] = 0;
      lg_post_core[iclus] = 0;

    }

  return 0;
}

int RcMpc09::process_event(PHCompositeNode *topNode)
{


  if (ievent % 10000 == 0){cout << "RcMpc09 event = " << ievent << endl;}
  ievent++;
  //if (ievent > 300000) return ABORTRUN;//Taking some chunk of data for calibrations
  phglobal = getClass<PHGlobal>(topNode,"PHGlobal");
  mpcclus        =      getClass<mpcClusterContainer>(topNode,"mpcClusterContainer");
  //  mpcraw         =      getClass<mpcRawContainer>(topNode, "MpcRaw2");
  mpctow         =      getClass<mpcTowerContainer>(topNode,"mpcTowerContainer");
  triglvl1       =      getClass<TrigLvl1>(topNode,"TrigLvl1");
  TriggerHelper trighelp(topNode);

  ResetVars();

  if (!mpctow) {cout << "Event " << ievent << "is missing mpctow node." << endl;}
  if (!mpcclus)  {cout << "Event " << ievent << "is missing mpcclus node." << endl;}
  event=ievent;
  const bool run9=true;
  const bool run13=!run9;
  if(run9){
    trig |= trighelp.trigScaled("BBCLL1(>0 tubes)") << 0; //minbias with vertex?
    trig |= trighelp.trigScaled("BBCLL1(>0 tubes) novertex)") <<1; //minbias no vertex
    trig |= trighelp.trigScaled("BBCLL1(>0 tubes) narrowvtx") << 2; //doesn't exist?
    trig |= trighelp.trigScaled("ZDCLL1wide") << 3;
    trig |= trighelp.trigScaled("MPC_4x4A") << 4; //equivalent of MPC_A in run13, I hope
    //trig |= trighelp.trigScaled("MPC_B") << 5; //doesn't exist in run9 pp500
  }
  else if (run13){//pp510
    trig |= trighelp.trigScaled("BBCLL1(>0 tubes)") << 0;
    trig |= trighelp.trigScaled("BBCLL1(>0 tubes) novertex") << 1;
    trig |= trighelp.trigScaled("BBCLL1(>0 tubes) narrowvtx") << 2;
    trig |= trighelp.trigScaled("ZDCLL1wide") << 3;
    trig |= trighelp.trigScaled("MPC_A") << 4;
    trig |= trighelp.trigScaled("MPC_B") << 5;
  }   
  // trig |= trighelp.trigScaled("MPC4x4c&ERTLL1_2x2") << 6;

  bbcvtx = phglobal->getBbcZVertex();
  zdcvtx = phglobal->getZdcZVertex();
  //if ( fabs(zvtx) > 150) return EVENT_OK;
  bunch = triglvl1->get_lvl1_clock_cross();

  //int pre_amu = mpcraw->get_pre_amu();
  //int post_amu = mpcraw->get_post_amu();

  //ntow = mpctow->size();
  for (int itow = 0; itow < MAXTOWERS; itow++)
    {
      chtow[itow] = -1;
      etow[itow]=-1;
    }
  nclus = 0;
  int towcount = 0;
  unsigned int size = mpcclus->size();
  for(unsigned int iclus=0; iclus<size; iclus++)
    {
      mpcClusterContent *clus = mpcclus->getCluster(iclus);
      TLorentzVector v = vectorize(clus);
      //cluster variables
      ecore[nclus] = clus->ecore();
      feecore[nclus] = clus->towerid(0);
      pt[nclus] = v.Pt();
      if (pt[nclus]<1.35) continue;
      x[nclus] = clus->x();
      y[nclus] = clus->y();
      z[nclus] = clus->z() - bbcvtx;
      dispx[nclus] = clus->dispx();
      dispy[nclus] = clus->dispy();
      disp[nclus] = TMath::Max(dispx[nclus],dispy[nclus]);
      chi2core[nclus] = clus->chi2core();
      mult[nclus] = clus->multiplicity();
      e8e9[nclus] = (clus->e9() - clus->ecent()) / clus->e9();
      cent_ch[nclus] = feecore[nclus];

      //central tower variables
      int cent_id = clus->towerid(0);
      //int tow_id = mpctow->findTower(cent_id);
      if ((cent_id < 0) || (cent_id > 575)) cout << "Channel num error." << endl;
      tdc_max[nclus] = clus->rawtdc();
      float lg_post = 0;
      for (int rtow = 0; rtow < mpctow->size(); rtow++){
	mpcTowerContent *tow = mpctow->getTower(rtow);
	if (tow->get_ch() == cent_id){
	  lg_post = tow->get_energy();
	  break;
	}
      }
      lg_post_core[nclus] = lg_post;
      nclus++;
    }

  ntow = towcount;

  if (nclus>0){
    T->Fill();
    if( T->GetEntries() % 10000 == 0 ) cout << "TTree Entries = " << T->GetEntries() << endl;
  }
  return EVENT_OK;
}

TLorentzVector RcMpc09::vectorize(mpcClusterContent *clus)
{
  double zvtx = phglobal->getBbcZVertex();
  double x = clus->x();
  double y = clus->y();
  double z = clus->z()-zvtx;
  double e = clus->ecore();

  double norm = sqrt(x*x+y*y+z*z);
  double px = e*x/norm;
  double py = e*y/norm;
  double pz = e*z/norm;

  TLorentzVector v;
  v.SetPxPyPzE(px,py,pz,e);
  return  v;
}
