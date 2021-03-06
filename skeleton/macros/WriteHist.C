#include <iostream>

void WriteHist(TString outfilename,vector<R00TLeSettings*> settings) {
  if (!gDirectory) return;
  TDirectory* lastdirectory = gDirectory;
  TFile* outfile = TFile::Open(outfilename, "RECREATE");
  if (!outfile || outfile->IsZombie()) {
    std::cout << "\033\[31mHistograms have not been written to file.\033\[39m" << std::endl;
    return;
  }

  for (int i=0;i<settings.size();i++){
    settings[i]->Write();
  }
  
  TList* list = lastdirectory->GetList() ;
  TIter next_object((TList*) list);
  TObject* obj ;
  while ((obj = next_object())) { 
    TString objname = obj->GetName();
    if (obj->InheritsFrom(TH1::Class())) {
      obj->Write();
    }
  }
  outfile->Write();
  outfile->Close();
  lastdirectory->cd();
  Info("WriteHist","Histograms have been written to %s",outfilename.Data());
}
