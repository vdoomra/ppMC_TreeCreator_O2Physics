#include "TFile.h"
#include "TChain.h"
#include "TKey.h"
#include "TList.h"
#include "TString.h"
#include "TTree.h"
#include <vector>
#include <algorithm>
#include <iostream>

void Convert2ROOTTree(const char* infile  = "tree_output_564356.root", const char* outfile = "eventTree_merged.root", const char* outTreeName = "eventTree")
{
    TFile* fin = TFile::Open(infile, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Could not open input file: " << infile << std::endl;
        return;
    }

    TChain chain("O2tree");
    TList* keys = fin->GetListOfKeys();
    TIter nextkey(keys);
    TKey* key;
    int nDF = 0;

    while ((key = (TKey*)nextkey())) {
        TString dirName = key->GetName();
        if (!dirName.BeginsWith("DF_")) continue;
        TString path = TString(infile) + "/" + dirName + "/O2tree";
        if (chain.Add(path) == 1) nDF++;
        else std::cerr << "Warning: failed to add " << path << std::endl;
    }

    fin->Close();

    if (nDF == 0) {
        std::cerr << "No DF_* directories with an O2tree found in " << infile << std::endl;
        return;
    }

    int nEntries = chain.GetEntries();
    std::cout << "Found " << nDF << " DF directories, " << nEntries << " total events." << std::endl;

    int maxDet = (int)std::max({ chain.GetMaximum("fDETPt_size"), chain.GetMaximum("fDETEta_size"), chain.GetMaximum("fDETPhi_size"), chain.GetMaximum("fDETCharge_size") });
    int maxTruth = (int)std::max({ chain.GetMaximum("fTruthPt_size"), chain.GetMaximum("fTruthEta_size"), chain.GetMaximum("fTruthPhi_size"), chain.GetMaximum("fTruthE_size"), chain.GetMaximum("fPDG_size") });

    const int MAXDET   = (int)maxDet   + 100;
    const int MAXTRUTH = (int)maxTruth + 100;

    float fZVTX, fWEIGHT, fPTHAT, fMULTIPLICITY;
    int fDETPt_size, fDETEta_size, fDETPhi_size, fDETCharge_size;
    int fTruthPt_size, fTruthEta_size, fTruthPhi_size, fTruthE_size, fPDG_size;

    std::vector<float> fDETPt(MAXDET), fDETEta(MAXDET), fDETPhi(MAXDET);
    std::vector<int>   fDETCharge(MAXDET);
    std::vector<float> fTruthPt(MAXTRUTH), fTruthEta(MAXTRUTH), fTruthPhi(MAXTRUTH), fTruthE(MAXTRUTH);
    std::vector<int>   fPDG(MAXTRUTH);

    chain.SetBranchAddress("fZVTX", &fZVTX);
    chain.SetBranchAddress("fWEIGHT", &fWEIGHT);
    chain.SetBranchAddress("fPTHAT", &fPTHAT);
    chain.SetBranchAddress("fMULTIPLICITY", &fMULTIPLICITY);

    chain.SetBranchAddress("fDETPt_size", &fDETPt_size);
    chain.SetBranchAddress("fDETPt", fDETPt.data());
    chain.SetBranchAddress("fDETEta_size", &fDETEta_size);
    chain.SetBranchAddress("fDETEta", fDETEta.data());
    chain.SetBranchAddress("fDETPhi_size", &fDETPhi_size);
    chain.SetBranchAddress("fDETPhi", fDETPhi.data());
    chain.SetBranchAddress("fDETCharge_size", &fDETCharge_size);
    chain.SetBranchAddress("fDETCharge", fDETCharge.data());

    chain.SetBranchAddress("fTruthPt_size", &fTruthPt_size);
    chain.SetBranchAddress("fTruthPt", fTruthPt.data());
    chain.SetBranchAddress("fTruthEta_size", &fTruthEta_size);
    chain.SetBranchAddress("fTruthEta", fTruthEta.data());
    chain.SetBranchAddress("fTruthPhi_size", &fTruthPhi_size);
    chain.SetBranchAddress("fTruthPhi", fTruthPhi.data());
    chain.SetBranchAddress("fTruthE_size", &fTruthE_size);
    chain.SetBranchAddress("fTruthE", fTruthE.data());
    chain.SetBranchAddress("fPDG_size", &fPDG_size);
    chain.SetBranchAddress("fPDG", fPDG.data());

    TFile* fout = TFile::Open(outfile, "RECREATE");
    TTree* outTree = new TTree(outTreeName, outTreeName);

    float o_zvtx, o_weight, o_ptHat, o_multiplicity;
    std::vector<float> det_pt, det_eta, det_phi;
    std::vector<int>   det_charge;
    std::vector<float> truth_pt, truth_eta, truth_phi, truth_e;
    std::vector<int>   pdg;

    outTree->Branch("vtx_z",       &o_zvtx,   "zvtx/F");
    outTree->Branch("weight",     &o_weight, "weight/F");
    outTree->Branch("ptHat",     &o_ptHat, "ptHat/F");
    outTree->Branch("multiplicity",     &o_multiplicity, "multiplicity/F");
    outTree->Branch("track_pt",     &det_pt);
    outTree->Branch("track_eta",    &det_eta);
    outTree->Branch("track_phi",    &det_phi);
    outTree->Branch("track_charge", &det_charge);
    outTree->Branch("truth_pt",   &truth_pt);
    outTree->Branch("truth_eta",  &truth_eta);
    outTree->Branch("truth_phi",  &truth_phi);
    outTree->Branch("truth_e",    &truth_e);
    outTree->Branch("pdg",        &pdg);

    for (int i = 0; i < nEntries; ++i) {

        chain.GetEntry(i);

        o_zvtx   = fZVTX;
        o_weight = fWEIGHT;
        o_ptHat = fPTHAT;
        o_multiplicity = fMULTIPLICITY;

        det_pt.assign(fDETPt.begin(),   fDETPt.begin()   + fDETPt_size);
        det_eta.assign(fDETEta.begin(), fDETEta.begin()  + fDETEta_size);
        det_phi.assign(fDETPhi.begin(), fDETPhi.begin()  + fDETPhi_size);
        det_charge.assign(fDETCharge.begin(), fDETCharge.begin() + fDETCharge_size);

        truth_pt.assign(fTruthPt.begin(),   fTruthPt.begin()   + fTruthPt_size);
        truth_eta.assign(fTruthEta.begin(), fTruthEta.begin()  + fTruthEta_size);
        truth_phi.assign(fTruthPhi.begin(), fTruthPhi.begin()  + fTruthPhi_size);
        truth_e.assign(fTruthE.begin(),     fTruthE.begin()    + fTruthE_size);
        pdg.assign(fPDG.begin(), fPDG.begin() + fPDG_size);

        outTree->Fill();

        if (i % 5000 == 0) std::cout << "  processed " << i << "/" << nEntries << " events\r" << std::flush;
    }
    std::cout << std::endl;

    fout->cd();
    outTree->Write();
    std::cout << "Wrote " << outTree->GetEntries() << " events to " << outfile << " (tree \"" << outTreeName << "\")" << std::endl;
    fout->Close();
}
