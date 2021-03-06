/**
 * This file is part of eelsmodel.
 *
 * eelsmodel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * eelsmodel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eelsmodel.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jo Verbeeck, Ruben Van Boxem
 * Copyright: 2002-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - fitters/lsqfitter.cpp
 **/

#include "src/fitters/lsqfitter.h"

#include <cstdio>

LSQFitter::LSQFitter(Model* m)
:Fitter(m)
{
 settype("LSQFitter");
}
LSQFitter::~LSQFitter(){
}
double LSQFitter::goodness_of_fit()const{
  //calculate the chi square
  double chisq=0.0;
  for (unsigned int i=0;i<modelptr->getnpoints();i++){
    if (!(modelptr->isexcluded(i))){//only take the non-excluded points
      const double exper=modelptr->getHLptr()->getcounts(i);
      const double fit=modelptr->getcounts(i);
      chisq+=pow((exper-fit),2.0);
      }
    }
  #ifdef FITTER_DEBUG
  std::cout <<"Chi Square: "<<chisq<<"\n";
  #endif
  return chisq;
}
std::string LSQFitter::goodness_of_fit_string()const{
  //returns a string which says how good the fit is
  char s[256];
  double chisq=goodness_of_fit();
  sprintf(s,"Chisq/dof: %e",chisq/(this->degreesoffreedom()));
  std::string f=s;
  return f;
}
void LSQFitter::calculate_beta_and_alpha(){
  //calculate beta and alpha matrix
  //clear lower left corner including diagonal
  for(int i = 0; i<alpha.rows(); ++i)
  {
    for(int j = 0; j<=i; ++j)
    {
      alpha(i,j)=0;
    }
  }
  //clear beta
  for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    beta[j]=0.0;
    }

  for (unsigned int i=0;i<(modelptr->getnpoints());i++){
    double expdata=(modelptr->getHLptr())->getcounts(i);
    double modeldata=modelptr->getcounts(i);
    if (!(modelptr->isexcluded(i))){ //don't count points that are excluded
      for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
            beta[j] += (expdata-modeldata)*deriv(j,i);
        for (size_t k=0; k<=j; k++){
         alpha(j,k) += deriv(j,i)*deriv(k,i);
         }
        }
      }
    }
  //copy the one triangle to the other side because of symmetry
  for (size_t j=1; j<modelptr->getnroffreeparameters(); j++){
  //was j=0 but first row needs not to be copied because is already full
      for (size_t k=0; k<j; k++){
        //was k<=j but you don't need to copy the diagonal terms
        alpha(k,j) = alpha(j,k);
        }
      }
}

double LSQFitter::likelihoodratio(){
  //calculate the likelyhood ratio (LR)
  //this number should be compared to the chi square distribution with
  //n-k degrees of freedom (n=number of points to be fitted, k number of parameters)
  //the model is a good approximation of the experiment if the likelyhood-ratio is to be compared
  //with the chi square cumulative distribution for a given confidence level
  double LR=0.0;
  for (size_t i=0;i<(modelptr->getnpoints());i++){
    if (!(modelptr->isexcluded(i))){
      //only take the non-excluded points
      const double exper=((modelptr->getHLptr())->getcounts(i));
      const double fit=(modelptr->getcounts(i));
      if ((fit>0.0)&&(exper>0.0)) {
        LR+=pow(exper-fit,2.0);//see paper Arian
        }
      }
    }
  #ifdef FITTER_DEBUG
  std::cout <<"likelihood ratio is: "<<LR<<"\n";
  #endif
  return LR;
}
