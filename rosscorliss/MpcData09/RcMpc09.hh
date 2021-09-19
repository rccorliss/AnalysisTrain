#ifndef __GENTREES_H__
#define __GENTREES_H__

#include "SubsysReco.h"

class Fun4AllHistoManager;
class PHCompositeNode;
class TH1F;
class TH2F;
class TH1D;

class MpcMap;
class MpcCalib;
class mpcRawContainer;
class mpcRawContent;
class mpcTowerContainer;
class mpcTowerContent;
class mpcClusterContainer;
class mpcClusterContent;
class TTree;
class TFile;
class TCanvas;
class TrigLvl1;

#include <TLorentzVector.h>
#include <PHGlobal.h>

using namespace std;

class RcMpc09: public SubsysReco
{
public:

  RcMpc09(char *_foutfile);
  virtual ~RcMpc09(){}

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
  int Reset(PHCompositeNode *topNode) { return 1; }
  TLorentzVector vectorize(mpcClusterContent *clus);
  int ResetVars();

protected:

  char *outfile;

  TFile *file;
  TTree *T;

  float bbcvtx;
  float zdcvtx;
  int nclus;
  int ntow;
  short bunch;
  short trig;

  const static int MAXCLUSTERS=100;
  const static int MAXTOWERS=1000;

  // float gains[576];
  int event;
  float ecore[MAXCLUSTERS];
  short feecore[MAXCLUSTERS];
  float pt[MAXCLUSTERS];
  float x[MAXCLUSTERS];
  float y[MAXCLUSTERS];
  float z[MAXCLUSTERS];

  short mult[MAXCLUSTERS];
  float disp[MAXCLUSTERS];
  float dispx[MAXCLUSTERS];
  float dispy[MAXCLUSTERS];
  float chi2core[MAXCLUSTERS];
  /*
       float lg_adc_core[MAXCLUSTERS];
          float hg_adc_core[MAXCLUSTERS];
  */
  short tdc_max[MAXCLUSTERS];
  short lg_post_core[MAXCLUSTERS];

  float etow[MAXTOWERS];
  short chtow[MAXTOWERS];
  MpcMap *mpcmap;
  int run;
  int ievent;

  // variables added by pmontu

  float e8e9[MAXCLUSTERS];
  int cent_ch[MAXCLUSTERS];

  MpcCalib *mpccalib;
  PHGlobal *phglobal;
  mpcClusterContainer *mpcclus;
  mpcTowerContainer *mpctow;
  // mpcRawContainer *mpcraw;
  TrigLvl1 *triglvl1;

};

#endif
