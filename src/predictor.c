//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"
#include <math.h> // For exponent

//
// TODO:Student Information
//
const char *studentName = "Enzo Han";
const char *studentID   = "A15519554";
const char *email       = "e3han@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

uint32_t *GHT_ptr; // Pointer for Global History Table
uint32_t ght_index;
uint32_t ghr; // Global History Register - Stores most recent outcomes
uint32_t ghr_mask; // 1's to AND ghr with to maintain ghistoryBits length
//
uint32_t lhp; // local history predictor
uint32_t lhp_mask;
uint32_t *LHT_ptr; // Local History Table 
uint32_t lht_index;
uint32_t *Ch_ptr; // Choice pointer, each index is a 2-bit counter 
uint32_t ch_index;

uint32_t *l_pht_ptr; // Local Pattern History Table
uint32_t l_pht_index;
uint32_t l_pht_mask;



//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  if(bpType == CUSTOM) {
    ghistoryBits = 12;
    lhistoryBits = 11;
    pcIndexBits = 11;
  }
  

  ghr = 0x00000000; // GHR stores the most recent output (path) 
  ghr_mask = 0xFFFFFFFF;
  ghr_mask = ghr_mask << (32 - ghistoryBits); 
  ghr_mask = ghr_mask >> (32 - ghistoryBits); // Stores ghistoryBits of 1's
  GHT_ptr = (uint32_t *)malloc((1<<ghistoryBits) * sizeof(uint32_t)); // think about uint8_t to save space?
  for(int i = 0; i < (1<<ghistoryBits); i++) {
    if(bpType == CUSTOM) {
      GHT_ptr[i] = 7;
    } else {
      GHT_ptr[i] = WN; // Initialize history to Weakly Not Taken (2-bit)
    }
  }
  lhp = 0x00000000;
  lhp_mask = 0xFFFFFFFF;
  lhp_mask = lhp_mask << (32 - pcIndexBits);
  lhp_mask = lhp_mask >> (32 - pcIndexBits);
  LHT_ptr = (uint32_t *)malloc((1<<pcIndexBits)*sizeof(uint32_t)); // figure out the right size for this
  for(int i = 0; i < (1<<pcIndexBits); i++) {
    LHT_ptr[i] = 0; // Initialize this to 0 but doesn't matter I believe
  }

  l_pht_ptr = (uint32_t *)malloc((1<<lhistoryBits)*sizeof(uint32_t)); // figure out right size for this
  for(int i = 0; i < (1<<lhistoryBits); i++) {
    if(bpType == CUSTOM) {
      l_pht_ptr[i] = 7; // Weakly Not 
    } else {
      l_pht_ptr[i] = WN; // Initialize each Local Pattern History counter to 01 (WN)
    }
  }
  l_pht_mask = 0xFFFFFFFF << (32 - lhistoryBits);
  l_pht_mask = l_pht_mask >> (32- lhistoryBits);

  Ch_ptr = (uint32_t *)malloc((1<<ghistoryBits)*sizeof(uint32_t));
  for(int i = 0; i < (1<<ghistoryBits); i++) {
    if(bpType == CUSTOM) {
      Ch_ptr[i] = 7;
    } else {
      Ch_ptr[i] = 1; // Initialize Chooser counter to 01 (I think its arbitrary)
    }
  }

}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //printf("PREDICT\n");
  //
  //TODO: Implement prediction scheme
  //
  uint32_t prediction; 
  uint32_t localPrediction;
  uint32_t globalPrediction;
  uint32_t choice;
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
      break;
    case GSHARE:
      ght_index = pc ^ ghr; 
      ght_index = ght_index & ghr_mask;
      prediction = GHT_ptr[ght_index];
      if( (prediction == WN) || (prediction == SN) ) {
        return NOTTAKEN;
      } else if( (prediction == WT) || (prediction == ST) ) {
        return TAKEN;
      } else {
        //printf("Error with prediction index");
        return NOTTAKEN;
      }
      break;
    case TOURNAMENT:
      // Local History is two-stages
      // Stage 1: PC Indexing
      lht_index = pc & lhp_mask; // 0x491245 that is of length pcIndexBits
      // Stage 2: Pattern History Indexing
      l_pht_index = LHT_ptr[lht_index]; // This gives me a pattern of 0/1's of length lhistorybits 
      localPrediction = l_pht_ptr[l_pht_index]; // Gives me a local prediction from 00 - 11
      // Global History 
      ght_index = ghr & ghr_mask; // Pattern of 0/1's of length ghistorybits 
      ch_index = ght_index; 
      globalPrediction = GHT_ptr[ght_index];
      choice = Ch_ptr[ght_index];
      if(choice > 1) { // 11 or 10 - Prediction Global
        prediction = globalPrediction;
      } else if(choice < 2) { // 01 or 00 Predict Local
        prediction = localPrediction;
      } else {
        printf("Error with choice");
        return NOTTAKEN;
      }
      if((prediction == WT) || (prediction == ST)) {
        return TAKEN;
      } else if((prediction == WN) || (prediction == SN)) {
        return NOTTAKEN;
      } else {
        printf("Prediction: %d\n", prediction);
        //printf("Error with prediction");
        return NOTTAKEN;
      }
      break;
    case CUSTOM:
      //lht_index = pc ^ ghr;
      lht_index = pc & lhp_mask; 
      // Stage 2: Pattern History Indexing
      l_pht_index = LHT_ptr[lht_index]; // This gives me a pattern of 0/1's of length lhistorybits 
      localPrediction = l_pht_ptr[l_pht_index]; // Gives me a local prediction from 00 - 11
      // Global History 
      ght_index = ghr & ghr_mask; // Pattern of 0/1's of length ghistorybits 
      ch_index = ght_index; 
      globalPrediction = GHT_ptr[ght_index];
      choice = Ch_ptr[ght_index];
      if(choice > 7) { // > 7 - Prediction Global
        prediction = globalPrediction;
      } else if(choice < 8) { // < 8 Predict Local
        prediction = localPrediction;
      } else {
        printf("Error with choice");
        return NOTTAKEN;
      }
      if(prediction > 7) {
        return TAKEN;
      } else if(prediction < 8) {
        return NOTTAKEN;
      } else {
        printf("Error with prediction");
        return NOTTAKEN;
      }
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //printf("TRAIN\n");
  uint32_t currentLocalPredict;
  uint32_t currentGlobalPredict;
  float theta;
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      ght_index = pc ^ ghr; 
      ght_index = ght_index & ghr_mask;
      uint32_t prediction = GHT_ptr[ght_index];
      if(outcome == 1) {
        ghr = ghr << 1;
        ghr = ghr + 1;
        ghr = ghr & ghr_mask;
        if((prediction == SN) || (prediction == WN) || (prediction == WT)) {
          GHT_ptr[ght_index] = prediction + 1;
        }
      } else if(outcome == 0) {
        ghr = ghr << 1;
        ghr = ghr + 0;
        ghr = ghr & ghr_mask;
        if((prediction == ST) || (prediction == WT) || (prediction == WN)) {
          GHT_ptr[ght_index] = prediction - 1;
        }
      } else {
        printf("Error! outcome wasn't 1 or 0");
      }
      break;
    case TOURNAMENT:
      lht_index = pc & lhp_mask; // Stage 1: PC Indexing
      l_pht_index = LHT_ptr[lht_index]; 
      currentLocalPredict = l_pht_ptr[l_pht_index];
      if(outcome == 1) {
        if(currentLocalPredict < 3) {
          l_pht_ptr[l_pht_index]++;
        }
      } else if(outcome == 0) {
        if(currentLocalPredict > 0) {
          l_pht_ptr[l_pht_index]--;
        }
      } else {
        printf("Error, outcome training 2");
        break;
      }
      //currentLocalPredict = l_pht_ptr[l_pht_index];
      if(currentLocalPredict > 1) {
        currentLocalPredict = TAKEN;
      } else {
        currentLocalPredict = NOTTAKEN;
      }
      // Global History 
      ghr = ghr & ghr_mask;
      ght_index = ghr & ghr_mask; // Global History Indexing
      ch_index = ght_index; 
      currentGlobalPredict = GHT_ptr[ght_index];
      if(outcome == 1) {
        if(currentGlobalPredict < 3) {
          GHT_ptr[ght_index]++;
        }
      } else if(outcome == 0) {
        if(currentGlobalPredict > 0) {
          GHT_ptr[ght_index]--; 
        }
      } else {
        printf("Error, outcome training 4");
      }
      //currentGlobalPredict = GHT_ptr[ght_index];
      if(currentGlobalPredict > 1) {
        currentGlobalPredict = TAKEN;
      } else {
        currentGlobalPredict = NOTTAKEN;
      }
      if(currentGlobalPredict == outcome && currentLocalPredict != outcome) {
        if(Ch_ptr[ght_index] < 3) {
          Ch_ptr[ght_index]++;
        }
      } else if(currentLocalPredict == outcome && currentGlobalPredict != outcome) {
        if(Ch_ptr[ght_index] > 0) {
          Ch_ptr[ght_index]--;
        }
      }
      l_pht_index = l_pht_index << 1; // include new outcome
      if(outcome == 1) {
        l_pht_index = l_pht_index + 1;
      } else if(outcome == 0) {
        l_pht_index = l_pht_index + 0;
      } else {
        printf("Error, outcome training");
        break;
      }
      l_pht_index = l_pht_index & lhp_mask;
      LHT_ptr[lht_index] = l_pht_index; // update pointer
      ghr = ghr << 1;
      if(outcome == 1) {
        ghr = ghr + 1;
      } else if(outcome == 0) {
        ghr = ghr + 0;
      } else {
        printf("Error, outcome training 3");
      }
      break;
    ///////////////////////////////////// -- CUSTOM -- //////////////////////////////////////////////////////////////////////////  
    case CUSTOM:
      //lht_index = pc ^ ghr; // Stage 1: PC Indexing
      lht_index = pc & lhp_mask;
      l_pht_index = LHT_ptr[lht_index]; 
      currentLocalPredict = l_pht_ptr[l_pht_index];
      if(outcome == 1) {
        if(currentLocalPredict < 15) {
          l_pht_ptr[l_pht_index]++;
        }
      } else if(outcome == 0) {
        if(currentLocalPredict > 0) {
          l_pht_ptr[l_pht_index]--;
        }
      } else {
        printf("Error, outcome training 2");
        break;
      }
      //currentLocalPredict = l_pht_ptr[l_pht_index];
      if(currentLocalPredict > 7) {
        currentLocalPredict = TAKEN;
      } else {
        currentLocalPredict = NOTTAKEN;
      }
      // Global History 
      ghr = ghr & ghr_mask;
      ght_index = ghr & ghr_mask; // Global History Indexing
      ch_index = ght_index; 
      currentGlobalPredict = GHT_ptr[ght_index];
      if(outcome == 1) {
        if(currentGlobalPredict < 15) {
          GHT_ptr[ght_index]++;
        }
      } else if(outcome == 0) {
        if(currentGlobalPredict > 0) {
          GHT_ptr[ght_index]--; 
        }
      } else {
        printf("Error, outcome training 4");
      }
      //currentGlobalPredict = GHT_ptr[ght_index];
      if(currentGlobalPredict > 7) {
        currentGlobalPredict = TAKEN;
      } else {
        currentGlobalPredict = NOTTAKEN;
      }
      if(currentGlobalPredict == outcome && currentLocalPredict != outcome) {
        if(Ch_ptr[ght_index] < 15) {
          Ch_ptr[ght_index]++;
        }
      } else if(currentLocalPredict == outcome && currentGlobalPredict != outcome) {
        if(Ch_ptr[ght_index] > 0) {
          Ch_ptr[ght_index]--;
        }
      }
      l_pht_index = l_pht_index << 1; // include new outcome
      if(outcome == 1) {
        l_pht_index = l_pht_index + 1;
      } else if(outcome == 0) {
        l_pht_index = l_pht_index + 0;
      } else {
        printf("Error, outcome training");
        break;
      }
      l_pht_index = l_pht_index & lhp_mask;
      LHT_ptr[lht_index] = l_pht_index; // update pointer
      ghr = ghr << 1;
      if(outcome == 1) {
        ghr = ghr + 1;
      } else if(outcome == 0) {
        ghr = ghr + 0;
      } else {
        printf("Error, outcome training 3");
      }
      break;
    default:
      break;
  }
}
