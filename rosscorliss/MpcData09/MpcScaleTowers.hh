#ifndef __MPCSCALETOWERS_H__
#define __MPCSCALETOWERS_H__

#include <SubsysReco.h>

class PHCompositeNode;
class mpcTowerContainer;
class MpcCalib;
class MpcMap;
//class recoConsts;

class MpcScaleTowers: public SubsysReco
{
public:
  MpcScaleTowers();
  virtual ~MpcScaleTowers();

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int EndRun(const int run);
  void SubtractDriverNoise(const int noisy_driver);


private:


  MpcCalib *mpccalib;
  MpcMap *mpcmap;
  mpcTowerContainer *mpctow;
  // float gain[576];

};

#endif 
