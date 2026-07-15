// test_posebust_core.cpp — dependency-free NativePoseQC unit tests
// Copyright 2026 Le Bonhomme Pharma
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "posebust/Engine.h"
#include "posebust/Types.h"

#include <cmath>
#include <string>

using namespace posebust;

namespace {

Molecule make_ethane_like() {
    Molecule m;
    m.name = "C2";
    // C-C single bond, tetrahedral-ish geometry (no H for simplicity)
    Atom c1;
    c1.id = 1;
    c1.element = "C";
    c1.atomic_num = 6;
    c1.x = 0.f;
    c1.y = 0.f;
    c1.z = 0.f;
    Atom c2 = c1;
    c2.id = 2;
    c2.x = 1.54f;
    m.atoms = {c1, c2};
    m.bonds = {Bond{0, 1, 1}};
    m.build_adjacency();
    return m;
}

Molecule make_protein_far_away() {
    Molecule p;
    p.name = "prot";
    Atom n;
    n.id = 1;
    n.element = "N";
    n.atomic_num = 7;
    n.x = 50.f;
    n.y = 0.f;
    n.z = 0.f;
    Atom ca = n;
    ca.id = 2;
    ca.element = "C";
    ca.atomic_num = 6;
    ca.x = 51.5f;
    Atom c = ca;
    c.id = 3;
    c.x = 53.f;
    p.atoms = {n, ca, c};
    p.bonds = {Bond{0, 1, 1}, Bond{1, 2, 1}};
    p.build_adjacency();
    return p;
}

}  // namespace

TEST(Types, Vec3Math) {
    Vec3 a{1, 0, 0}, b{0, 1, 0};
    EXPECT_FLOAT_EQ(dot(a, b), 0.f);
    EXPECT_NEAR(norm(a), 1.f, 1e-6f);
    EXPECT_NEAR(dist(a, b), std::sqrt(2.f), 1e-5f);
}

TEST(Types, ReportFailClosedEmpty) {
    PoseBustReport r;
    EXPECT_FALSE(r.all_passed());
    EXPECT_FALSE(r.native_qc_diagnostic_pass());
    EXPECT_FALSE(r.success_pb_full());
}

TEST(Engine, EvaluateSimpleNoClash) {
    const Molecule lig = make_ethane_like();
    const Molecule prot = make_protein_far_away();
    EvaluateOptions opt;
    opt.suite = Suite::Dock;
    auto report = evaluate(lig, prot, nullptr, opt);
    ASSERT_TRUE(report.ran) << report.error;
    EXPECT_TRUE(report.error.empty()) << report.error;
    EXPECT_EQ(report.backend, "native_pose_qc");
    EXPECT_GT(report.n_checks(), 0);
    // Diagnostic keys present
    EXPECT_NE(report.find_check("mol_pred_loaded"), nullptr);
    EXPECT_NE(report.find_check("internal_steric_clash"), nullptr);
}

TEST(Engine, BackendEnvNative) {
    // Note: process-wide env; restore after.
    const char* prev = std::getenv("POSEBUST_BACKEND");
    setenv("POSEBUST_BACKEND", "native", 1);
    EXPECT_EQ(resolve_backend_from_env(), Backend::Native);
    setenv("POSEBUST_BACKEND", "off", 1);
    EXPECT_EQ(resolve_backend_from_env(), Backend::Off);
    if (prev)
        setenv("POSEBUST_BACKEND", prev, 1);
    else
        unsetenv("POSEBUST_BACKEND");
}
