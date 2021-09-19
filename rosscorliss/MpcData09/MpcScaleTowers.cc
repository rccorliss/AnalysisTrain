#include <iostream>
#include <fstream>
#include <algorithm>

#include "MpcScaleTowers.hh"
#include <getClass.h>
#include <mpcTowerContainer.h>
#include <mpcTowerContent.h>
#include <Fun4AllReturnCodes.h>
#include "TOAD.h"
#include "MpcCalib.h"
#include "MpcMap.h"
#include "recoConsts.h"

using namespace std;
using namespace findNode;

MpcScaleTowers::MpcScaleTowers(): SubsysReco("MpcScaleTowers")
{
  mpccalib = MpcCalib::instance();
  mpcmap = MpcMap::instance();
  mpctow = new mpcTowerContainer;
}

MpcScaleTowers::~MpcScaleTowers()
{
}

int MpcScaleTowers::Init(PHCompositeNode *topNode)
{
  return 0;
}

int MpcScaleTowers::EndRun(const int runnumber)
{
  return 0;
}

int MpcScaleTowers::InitRun(PHCompositeNode *topNode)
{
  recoConsts *rc = recoConsts::instance();
  int irun = rc->get_IntFlag("RUNNUMBER");
  cout << "run = " << irun << endl;

  // for(int fee576ch=0; fee576ch<576; fee576ch++)
  //   {
  //     gain[fee576ch]=-1;
  //   }

  // int ich;
  // float igain;
  // TOAD *toad_loader = new TOAD("SkimMpc11");
  // string file_location = toad_loader->location(// "/gpfs/mnt/gpfs02/phenix/spin/spin2/"
  // 					       // "pmontu/offline/AnalysisTrain/SkimMpcData11/"
  // 					       "final_gain_ratios.txt");
  // // ifstream gain_in(file_location.c_str());
  // ifstream gain_in("/phenix/spin2/"                                            
  // 		   "pmontu/offline/AnalysisTrain/SkimMpcData11/"                                    
  // 		   "final_gain_ratios.txt");
  // cout << "Reading gains from " << file_location.c_str() << endl;
  // if (!gain_in.good()){cout << "Error opening gain file." << endl;}
  // while (gain_in.good()){
  //   gain_in >> ich >> igain;
  //   gain[ich] = igain;
  // }
  // for (int i = 0;i < 576;i++){cout << gain[i] << "\t";}
  // gain_in.close();
  // delete toad_loader;
  return 0;
}

int MpcScaleTowers::process_event(PHCompositeNode *topNode)
{
  mpctow = findNode::getClass<mpcTowerContainer>(topNode,"mpcTowerContainer");
  
  if (!mpctow)
    {
      cout << PHWHERE << "Unable to get mpcTowerContainer, is Node missing?" << endl;
      return EVENT_OK;
    }
    SubtractDriverNoise(19);
    for(unsigned int itow=0; itow<mpctow->size(); itow++)
    {
       mpcTowerContent *tower = mpctow->getTower(itow);
       // int fee576ch = tower->get_ch();
       float energy = tower->get_energy();
       // if (gain[fee576ch] <= 0){gain[fee576ch] = 1.05;} //this is an averageish value in case something has gone wrong with an individual tower in the gain file 
       // //if (mpcmap->getDriver(fee576ch) == 19) {gain[fee576ch] = 1.2;}//driver's got problems, so no reliable gains yet
       // energy *= (gain[fee576ch]); 
       tower->set_energy(energy);
    }
    
  return EVENT_OK;
}

void MpcScaleTowers::SubtractDriverNoise(const int noisy_driver)
{
  vector<float> driver_e;
  vector<float> driver_ich;
  double mean = 0.;
  for (unsigned int ich = 0; ich < mpctow->size(); ich++)
    {
      mpcTowerContent *tower = mpctow->getTower(ich);

      int fee576ch = tower->get_ch();
      if ( mpcmap->isCrystal( fee576ch ) == 0 ) continue;

      int driver = mpcmap->getDriver( fee576ch );
      if ( driver!=noisy_driver ) continue;

      float energy = tower->get_energy();

      driver_e.push_back( energy );
      driver_ich.push_back( ich );
      mean += energy;
    }

  std::sort( driver_e.begin(), driver_e.end() );
  int ntowers = driver_e.size();
  if ( ntowers>10 )
    {
      mean = mean/ntowers;
      double trunc_mean = 0.;
      for (int i=2; i<ntowers-2; i++)
        {
          trunc_mean += driver_e[i];
          //cout << "trunc_mean " << i << "\t" << driver_e[i] << endl; 
        }
      trunc_mean = trunc_mean/(ntowers-4);
/*
      cout << "DRIVER1E " << ntowers << " " << driver_e[0]
           << "\t" << driver_e[ntowers-1]
           << "\t" << mean
           << "\t" << trunc_mean
           << endl;
*/

      for (unsigned int i=0; i<driver_ich.size(); i++)
        {
          int ich = driver_ich[i];
          mpcTowerContent *tower = mpctow->getTower(ich);
          float e = tower->get_energy();
          e -= trunc_mean;
          tower->set_energy( e );
        }
    }
}
