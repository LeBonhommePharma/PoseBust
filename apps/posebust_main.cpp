// posebust — standalone C++26 pose validation CLI
//
// Copyright 2026 Le Bonhomme Pharma
// SPDX-License-Identifier: Apache-2.0
//
// Usage:
//   posebust --native --pred ligand.sdf --protein receptor.pdb [-l crystal.sdf]
//   posebust --bust   --pred ligand.sdf --protein receptor.pdb [-l crystal.sdf]
//   posebust --paths  complex.pdb receptor.pdb [crystal.sdf]   # FlexAID pose path
//
// Default: --native (clean-room NativePoseQC; no Python). Use --bust for the
// official upstream PoseBusters CLI when installed (pb_pass claim gate).

#include "posebust/BustCli.h"
#include "posebust/Engine.h"
#include "posebust/Loaders.h"
#include "posebust/Types.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

void print_usage(const char* argv0) {
    std::cerr
        << "PoseBust " << "0.1.0"
        << " — standalone C++26 pose validation (Apache-2.0)\n\n"
        << "Usage:\n"
        << "  " << argv0
        << " --native --pred <lig.sdf|pdb> --protein <rec.pdb> [-l <crystal.sdf>]\n"
        << "  " << argv0
        << " --bust   --pred <lig.sdf> --protein <rec.pdb> [-l <crystal.sdf>]\n"
        << "  " << argv0
        << " --paths  <complex.pdb> <receptor.pdb> [crystal.sdf]\n\n"
        << "Options:\n"
        << "  --native          Run clean-room NativePoseQC (default)\n"
        << "  --bust            Run upstream PoseBusters CLI (`bust`)\n"
        << "  --paths           Extract FlexAID-style ligand from complex PDB\n"
        << "  --pred PATH       Predicted ligand (SDF preferred)\n"
        << "  --protein PATH    Protein PDB\n"
        << "  -l, --crystal PATH  Optional crystal/reference SDF (topology)\n"
        << "  --sidecar DIR     Write JSON/CSV sidecars under DIR\n"
        << "  --suite dock|redock|mol   Native suite (default: dock)\n"
        << "  -h, --help        This help\n\n"
        << "Environment:\n"
        << "  POSEBUSTERS_BIN / FLEXAIDDS_POSEBUSTERS_BIN  path to `bust`\n"
        << "  POSEBUST_BACKEND=native|bust|off\n"
        << "  POSEBUST_INCHI_BIN                          optional inchi-1\n";
}

void print_report(const posebust::PoseBustReport& r) {
    std::cout << "backend=" << r.backend
              << " ran=" << (r.ran ? 1 : 0)
              << " n_checks=" << r.n_checks()
              << " n_pass=" << r.n_pass()
              << " n_fail=" << r.n_fail()
              << " native_qc_diagnostic="
              << (r.native_qc_diagnostic_pass() ? "PASS" : "FAIL")
              << " full=" << (r.success_pb_full() ? "PASS" : "FAIL")
              << "\n";
    if (!r.error.empty()) std::cout << "error=" << r.error << "\n";
    if (!r.warning.empty()) std::cout << "warning=" << r.warning << "\n";
    for (const auto& c : r.checks) {
        std::cout << "  [" << (c.passed ? "PASS" : "FAIL") << "] "
                  << c.key;
        if (!c.detail.empty()) std::cout << "  " << c.detail;
        std::cout << "\n";
    }
    if (!r.failed_keys_csv().empty())
        std::cout << "failed_keys=" << r.failed_keys_csv() << "\n";
}

int run_native_paths(const std::string& complex_pdb,
                     const std::string& receptor_pdb,
                     const std::string& crystal_sdf,
                     const posebust::EvaluateOptions& opt) {
    auto report = posebust::evaluate_paths(complex_pdb, receptor_pdb, crystal_sdf, opt);
    print_report(report);
    if (!opt.sidecar_dir.empty()) {
        std::string err;
        const auto jp = std::filesystem::path(opt.sidecar_dir) / "posebust_report.json";
        if (!posebust::write_report_json(report, jp.string(), &err)) {
            std::cerr << "write_report_json failed: " << err << "\n";
        } else {
            std::cout << "json=" << jp.string() << "\n";
        }
    }
    return report.success_pb_full() ? 0 : 1;
}

int run_native_files(const std::string& pred,
                     const std::string& protein,
                     const std::string& crystal,
                     const posebust::EvaluateOptions& opt) {
    posebust::Molecule lig, prot, ref;
    std::string err;
    const auto pred_ext = std::filesystem::path(pred).extension().string();
    bool ok_lig = false;
    if (pred_ext == ".sdf" || pred_ext == ".mol" || pred_ext == ".MOL" || pred_ext == ".SDF") {
        ok_lig = posebust::load_sdf(pred, lig, &err);
    } else {
        ok_lig = posebust::load_pdb_ligand(pred, lig, &err);
    }
    if (!ok_lig) {
        std::cerr << "load pred failed: " << err << "\n";
        return 2;
    }
    if (!posebust::load_pdb_protein_heavy(protein, prot, &err)) {
        std::cerr << "load protein failed: " << err << "\n";
        return 2;
    }
    posebust::Molecule* pref = nullptr;
    if (!crystal.empty()) {
        if (!posebust::load_sdf(crystal, ref, &err)) {
            std::cerr << "load crystal failed: " << err << "\n";
            return 2;
        }
        // Topology from crystal when heavy elements match
        std::string terr;
        if (!posebust::assign_topology_from_reference(lig, ref, &terr) && !terr.empty()) {
            std::cerr << "topology warning: " << terr << "\n";
        }
        pref = &ref;
    }
    auto report = posebust::evaluate(lig, prot, pref, opt);
    print_report(report);
    if (!opt.sidecar_dir.empty()) {
        std::string jerr;
        const auto jp = std::filesystem::path(opt.sidecar_dir) / "posebust_report.json";
        if (posebust::write_report_json(report, jp.string(), &jerr))
            std::cout << "json=" << jp.string() << "\n";
    }
    return report.success_pb_full() ? 0 : 1;
}

int run_bust(const std::string& pred,
             const std::string& protein,
             const std::string& crystal,
             const std::string& sidecar) {
    auto r = posebust::run_upstream_bust(pred, protein, crystal, sidecar, "pose");
    std::cout << "backend=" << r.backend
              << " ran=" << (r.ran ? 1 : 0)
              << " pb_pass=" << (r.pb_pass ? "PASS" : "FAIL")
              << " n_checks=" << r.n_checks
              << " n_pass=" << r.n_pass
              << " n_fail=" << r.n_fail
              << "\n";
    if (!r.error.empty()) std::cout << "error=" << r.error << "\n";
    if (!r.failed_keys.empty()) std::cout << "failed_keys=" << r.failed_keys << "\n";
    if (!r.csv_path.empty()) std::cout << "csv=" << r.csv_path << "\n";
    return r.pb_pass ? 0 : 1;
}

}  // namespace

int main(int argc, char** argv) {
    bool use_bust = false;
    bool use_paths = false;
    bool use_native = false;
    std::string pred, protein, crystal, sidecar, complex_pdb, receptor_pdb;
    posebust::EvaluateOptions opt;
    opt.suite = posebust::Suite::Dock;

    std::vector<std::string> positional;
    for (int i = 1; i < argc; ++i) {
        const std::string_view a = argv[i];
        auto need = [&](const char* name) -> std::string {
            if (i + 1 >= argc) {
                std::cerr << "missing value for " << name << "\n";
                std::exit(2);
            }
            return argv[++i];
        };
        if (a == "-h" || a == "--help") {
            print_usage(argv[0]);
            return 0;
        }
        if (a == "--native") {
            use_native = true;
            continue;
        }
        if (a == "--bust") {
            use_bust = true;
            continue;
        }
        if (a == "--paths") {
            use_paths = true;
            continue;
        }
        if (a == "--pred") {
            pred = need("--pred");
            continue;
        }
        if (a == "--protein") {
            protein = need("--protein");
            continue;
        }
        if (a == "-l" || a == "--crystal") {
            crystal = need("--crystal");
            continue;
        }
        if (a == "--sidecar") {
            sidecar = need("--sidecar");
            opt.sidecar_dir = sidecar;
            continue;
        }
        if (a == "--suite") {
            const auto s = need("--suite");
            if (s == "dock") opt.suite = posebust::Suite::Dock;
            else if (s == "redock") opt.suite = posebust::Suite::Redock;
            else if (s == "mol") opt.suite = posebust::Suite::Mol;
            else {
                std::cerr << "unknown suite: " << s << "\n";
                return 2;
            }
            continue;
        }
        if (!a.empty() && a[0] == '-') {
            std::cerr << "unknown option: " << a << "\n";
            print_usage(argv[0]);
            return 2;
        }
        positional.emplace_back(a);
    }

    if (!use_bust && !use_paths && !use_native) use_native = true;

    if (use_paths) {
        if (positional.size() < 2) {
            std::cerr << "--paths needs <complex.pdb> <receptor.pdb> [crystal.sdf]\n";
            return 2;
        }
        complex_pdb = positional[0];
        receptor_pdb = positional[1];
        if (positional.size() >= 3) crystal = positional[2];
        return run_native_paths(complex_pdb, receptor_pdb, crystal, opt);
    }

    if (use_bust) {
        if (pred.empty() || protein.empty()) {
            std::cerr << "--bust requires --pred and --protein\n";
            return 2;
        }
        return run_bust(pred, protein, crystal, sidecar);
    }

    // native file mode
    if (pred.empty() || protein.empty()) {
        // allow positional: pred protein [crystal]
        if (positional.size() >= 2) {
            pred = positional[0];
            protein = positional[1];
            if (positional.size() >= 3) crystal = positional[2];
        } else {
            print_usage(argv[0]);
            return 2;
        }
    }
    return run_native_files(pred, protein, crystal, opt);
}
