
#include <vector>

#include "LendaEvent.hh"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TDirectory.h"

void OverPlotLE(Long64_t entry=0,Double_t offset=1660,int BarNum=0){


  LendaEvent * event = new LendaEvent();
  
  TTree* tree =(TTree*)gDirectory->Get("caltree");
  if ( tree == NULL){
    cout<<"Can't find caltree.  Meant to be run from within calibrated ROOT file"<<endl;
    return;
  }

  tree->SetBranchAddress("lendaevent",&event);
  
  TCanvas *c = new TCanvas("c11");
  TCanvas *c1 = new TCanvas("c21");


  tree->GetEntry(entry);

  Int_t NumBars = event->NumBars;
  
  if (BarNum >= NumBars){
    cout<<"Event has "<<NumBars<<" can't plot bar "<<BarNum<<endl;
    return;
  }
  
  Int_t NumTops = event->Bars[BarNum].NumTops;
  Int_t NumBottoms = event->Bars[BarNum].NumBottoms;

  Int_t largerNum;

  if (NumTops>NumBottoms){
    largerNum=NumTops;
  }else {
    largerNum=NumBottoms;
  }

  
  c->Divide(1,largerNum);
  c1->Divide(1,largerNum);

  int traceSize=-1000;  
  int filterSize=-1000;  
  int cfdSize=-1000;  

  if (NumTops != 0){
    traceSize = event->Bars[BarNum].Tops[0].GetTrace().size();
    filterSize = event->Bars[BarNum].Tops[0].GetFilter().size();
    cfdSize = event->Bars[BarNum].Tops[0].GetCFD().size();
  } else if (NumBottoms !=0){
    traceSize = event->Bars[BarNum].Bottoms[0].GetTrace().size();
    filterSize = event->Bars[BarNum].Tops[0].GetFilter().size();
    cfdSize = event->Bars[BarNum].Tops[0].GetCFD().size();
  }

  cout<<"TRACE SIZE IS "<<traceSize<<endl;
  cout<<"Filter Size is "<<filterSize<<endl;
  cout<<"CFD Size is "<<cfdSize<<endl;

  Double_t *x = (Double_t*)malloc(traceSize*sizeof(Double_t));
  
  for (int i=0;i<traceSize;i++){
    x[i]=i;
  }
  

  std::vector <TGraph*> BottomTraces;
  BottomTraces.resize(NumBottoms);

  std::vector <TGraph*> TopTraces;
  TopTraces.resize(NumTops);


  std::vector <TGraph*> BottomCFDs;
  BottomCFDs.resize(NumBottoms);

  std::vector <TGraph*> TopCFDs;
  TopCFDs.resize(NumTops);


  std::vector <TGraph*> TopFilters;
  TopFilters.resize(NumTops);

  std::vector <TGraph*> BottomFilters;
  BottomFilters.resize(NumTops);



  Double_t * temp =(Double_t*)malloc(traceSize*sizeof(Double_t));

  for (int i=0;i<NumBottoms;i++){

    for (int j=0;j<traceSize;j++){
      temp[j]=event->Bars[BarNum].Bottoms[i].GetTrace()[j]-offset;
    }
    BottomTraces[i] = new TGraph(traceSize,x,temp);
    BottomTraces[i]->SetTitle((event->Bars[BarNum].Bottoms[i].GetChannelName()+string(" Trace")).c_str());
    BottomTraces[i]->SetName(event->Bars[BarNum].Bottoms[i].GetChannelName().c_str());

    
    for (int j=0;j<cfdSize;j++){
      temp[j]=event->Bars[BarNum].Bottoms[i].GetCFD()[j];
    }
    BottomCFDs[i] = new TGraph(cfdSize,x,temp);
    BottomCFDs[i]->SetTitle((event->Bars[BarNum].Bottoms[i].GetChannelName()+string(" CFD")).c_str());
    BottomCFDs[i]->SetName(event->Bars[BarNum].Bottoms[i].GetChannelName().c_str());


    for (int j=0;j<filterSize;j++){
      temp[j]=event->Bars[BarNum].Bottoms[i].GetFilter()[j];
    }
    BottomFilters[i] = new TGraph(filterSize,x,temp);
    BottomFilters[i]->SetTitle((event->Bars[BarNum].Bottoms[i].GetChannelName()+string(" Filter")).c_str());
    BottomFilters[i]->SetName(event->Bars[BarNum].Bottoms[i].GetChannelName().c_str());



  }
  

  for (int i=0;i<NumTops;i++){
    for (int j=0;j<traceSize;j++){
      temp[j]=event->Bars[BarNum].Tops[i].GetTrace()[j]-offset;
    }
    TopTraces[i] = new TGraph(traceSize,x,temp);
    TopTraces[i]->SetTitle((event->Bars[BarNum].Tops[i].GetChannelName()+string(" Trace")).c_str());
    TopTraces[i]->SetName(event->Bars[BarNum].Tops[i].GetChannelName().c_str());
    
    for (int j=0;j<cfdSize;j++){
      temp[j]=event->Bars[BarNum].Tops[i].GetCFD()[j];
    }
    TopCFDs[i] = new TGraph(cfdSize,x,temp);
    TopCFDs[i]->SetTitle((event->Bars[BarNum].Tops[i].GetChannelName()+string(" CFD")).c_str());
    TopCFDs[i]->SetName(event->Bars[BarNum].Tops[i].GetChannelName().c_str());


    for (int j=0;j<filterSize;j++){
      temp[j]=event->Bars[BarNum].Tops[i].GetFilter()[j];
    }
    TopFilters[i] = new TGraph(filterSize,x,temp);
    TopFilters[i]->SetTitle((event->Bars[BarNum].Tops[i].GetChannelName()+string(" Filter")).c_str());
    TopFilters[i]->SetName(event->Bars[BarNum].Tops[i].GetChannelName().c_str());
 
  }






  free(temp);


  LendaFilter theFilt;
  int topSpot=-1;
  vector <Double_t> coefs = theFilt.
    GetMatrixInversionAlgorithmCoeffients(event->Bars[BarNum].Tops[0].GetCFD(),topSpot);
  cout<<"TopSpot is "<<topSpot<<endl;
  TF1 * f = new TF1("topFunc","pol3",topSpot-1,topSpot+2);
  for (int i=0;i<coefs.size();i++){
    cout<<coefs[i]<<endl;
    f->SetParameter(i,coefs[3-i]);
  }

  //gStyle->SetMarkerStyle(8);
  //  gStyle->UseCurrentStyle();
  for (int i=0;i<NumBottoms;i++){
    c->cd(i+1);
    BottomCFDs[i]->SetLineColor(kBlue);
    BottomCFDs[i]->SetMarkerColor(kBlue);
    BottomCFDs[i]->SetFillColor(kBlue);
    BottomCFDs[i]->SetLineWidth(2);
    BottomCFDs[i]->GetHistogram()->GetXaxis()->SetTitle("Clock Ticks");
    BottomCFDs[i]->GetHistogram()->GetYaxis()->SetTitle("ADC Units");
    BottomCFDs[i]->SetMarkerStyle(8);
    if (cfdSize!=0){BottomCFDs[i]->Draw("APLsame");}

    if(traceSize!=0){BottomTraces[i]->Draw("sameLp");}
    BottomTraces[i]->SetFillColor(kBlack);
    BottomTraces[i]->SetLineWidth(2);
    BottomTraces[i]->SetMarkerStyle(8);

    BottomFilters[i]->SetLineColor(kRed);
    BottomFilters[i]->SetMarkerColor(kRed);
    BottomFilters[i]->SetFillColor(kRed);
    BottomFilters[i]->SetLineWidth(2);
    BottomFilters[i]->SetMarkerStyle(8);
    if (filterSize!=0){BottomFilters[i]->Draw("samePL");}


    TLegend * leg = new TLegend(0.8,0.8,0.9,0.9);
    leg->SetFillColor(kWhite);
    //    leg->SetTextSize(1);
    leg->SetLineWidth(2);
    leg->AddEntry(BottomTraces[i],"Trace","f");
    leg->AddEntry(BottomCFDs[i],"CFD","f");
    leg->AddEntry(BottomFilters[i],"Filter","f");
    leg->Draw();
  }
  for (int i=0;i<NumTops;i++){
    c1->cd(i+1);

    TopCFDs[i]->SetLineColor(kBlue);
    TopCFDs[i]->SetMarkerColor(kBlue);
    TopCFDs[i]->SetFillColor(kBlue);
    TopCFDs[i]->SetLineWidth(2);
    TopCFDs[i]->GetHistogram()->GetXaxis()->SetTitle("Clock Ticks");
    TopCFDs[i]->GetHistogram()->GetYaxis()->SetTitle("ADC Units");
    TopCFDs[i]->SetMarkerStyle(8);
    if(cfdSize!=0){TopCFDs[i]->Draw("APLsame");f->Draw("same");}
    
    TopFilters[i]->SetLineColor(kRed);
    TopFilters[i]->SetMarkerColor(kRed);
    TopFilters[i]->SetFillColor(kRed);
    TopFilters[i]->SetLineWidth(2);
    TopFilters[i]->SetMarkerStyle(8);
    //if (filterSize!=0){TopFilters[i]->Draw("samePL");}

    TopTraces[i]->SetFillColor(kBlack);
    TopTraces[i]->SetLineWidth(2);
    TopTraces[i]->SetMarkerStyle(8);
    if(traceSize!=0){TopTraces[i]->Draw("sameLP");}

    TLegend * leg = new TLegend(0.8,0.8,0.9,0.9);
    leg->SetFillColor(kWhite);
    //    leg->SetTextSize(1);
    leg->SetLineWidth(2);
    leg->AddEntry(TopTraces[i],"Trace","f");
    leg->AddEntry(TopCFDs[i],"CFD","f");
    leg->AddEntry(TopFilters[i],"Filter","f");
    leg->Draw();
    
  }

}

//   Double_t* x = malloc(size*sizeof(Double_t));
  
//   Double_t* y = malloc(size*sizeof(Double_t));

//   Double_t* y1= malloc(size*sizeof(Double_t));
  
//   Double_t* y2=malloc(size*sizeof(Double_t));
    

//   cout<<"size is "<<size<<endl;
//   for (int i=0;i<size;i++){

//     x[i]=i;
//     y[i]=event->Bars[0].Tops[0].GetTrace()[i];
//     y1[i]=event->Bars[0].Tops[0].GetFilter()[i];
//     y2[i]=event->Bars[0].Tops[0].GetCFD()[i];
//   }

//   TGraph *gr = new TGraph(size,x,y);
//   TGraph *gr1 = new TGraph(size,x,y1);
//   TGraph *gr2 = new TGraph(size,x,y2);
    
//   stringstream s;
//   s<<"Traces row "<<j<<" chan "<<event->Bars[0].Name;
//   gr->SetTitle(s.str().c_str());
//   s.str(""); s<<"Filters row "<<j<<" chan "<<event->Bars[0].Name;
//   gr1->SetTitle(s.str().c_str());
//   s.str(""); s<<"CFDs row"<<j<<" chan "<<event->Bars[0].Name;
//   gr2->SetTitle(s.str().c_str());

//   if(num!=1){
//     c->cd(j+1-entry);
//     gr->Draw("AL*");
      
//     c1->cd(j+1-entry);
//     gr1->Draw("AL*");
      
//     c2->cd(j+1-entry);
//     gr2->Draw("AL*");
//   } else{


//     c->cd();
//     gr->Draw("AL*");

//     c1->cd();
//     gr1->Draw("AL*");

//     c2->cd();
//     gr2->Draw("AL*");

//   }
  
// }
