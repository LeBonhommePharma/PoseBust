// test_posebust_loaders.cpp — SDF/PDB round-trip tests (self-contained fixtures)
// Copyright 2026 Le Bonhomme Pharma
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "posebust/Loaders.h"

#include <filesystem>
#include <fstream>
#include <string>

using namespace posebust;
namespace fs = std::filesystem;

namespace {

fs::path write_temp(const std::string& name, const std::string& content) {
    const fs::path p = fs::temp_directory_path() / name;
    std::ofstream ofs(p);
    ofs << content;
    return p;
}

// Minimal V2000 methane-like single carbon (no H)
const char* kTinySdf = R"(tiny
  PoseBust

  1  0  0  0  0  0  0  0  0  0999 V2000
    0.0000    0.0000    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0
M  END
$$$$
)";

const char* kEthaneSdf = R"(ethane
  PoseBust

  2  1  0  0  0  0  0  0  0  0999 V2000
    0.0000    0.0000    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0
    1.5400    0.0000    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0
  1  2  1  0  0  0  0
M  END
$$$$
)";

}  // namespace

TEST(Loaders, AtomicNumberBasics) {
    EXPECT_EQ(atomic_number("C"), 6);
    EXPECT_EQ(atomic_number("Cl"), 17);
    EXPECT_EQ(atomic_number("CL"), 17);
    EXPECT_EQ(atomic_number("H"), 1);
    EXPECT_EQ(atomic_number("Xx"), 0);
}

TEST(Loaders, LoadSdfEthane) {
    const auto path = write_temp("posebust_ethane.sdf", kEthaneSdf);
    Molecule m;
    std::string err;
    ASSERT_TRUE(load_sdf(path.string(), m, &err)) << err;
    EXPECT_EQ(m.atoms.size(), 2u);
    EXPECT_EQ(m.bonds.size(), 1u);
    EXPECT_EQ(m.n_heavy(), 2);
    EXPECT_EQ(m.atoms[0].element, "C");
    EXPECT_NEAR(m.atoms[1].x, 1.54f, 1e-3f);
    fs::remove(path);
}

TEST(Loaders, WriteSdfRoundTrip) {
    const auto path = write_temp("posebust_tiny.sdf", kTinySdf);
    Molecule m;
    std::string err;
    ASSERT_TRUE(load_sdf(path.string(), m, &err)) << err;
    const auto out = fs::temp_directory_path() / "posebust_roundtrip.sdf";
    ASSERT_TRUE(write_sdf(m, out.string(), &err)) << err;
    Molecule m2;
    ASSERT_TRUE(load_sdf(out.string(), m2, &err)) << err;
    EXPECT_EQ(m2.atoms.size(), m.atoms.size());
    fs::remove(path);
    fs::remove(out);
}

TEST(Loaders, TopologyAssignMatch) {
    Molecule pred, ref;
    std::string err;
    const auto p1 = write_temp("posebust_pred.sdf", kEthaneSdf);
    const auto p2 = write_temp("posebust_ref.sdf", kEthaneSdf);
    ASSERT_TRUE(load_sdf(p1.string(), pred, &err)) << err;
    ASSERT_TRUE(load_sdf(p2.string(), ref, &err)) << err;
    // clear bonds on pred to force assign
    pred.bonds.clear();
    pred.adj.clear();
    ASSERT_TRUE(assign_topology_from_reference(pred, ref, &err)) << err;
    EXPECT_EQ(pred.bonds.size(), 1u);
    fs::remove(p1);
    fs::remove(p2);
}

TEST(Loaders, TopologyMismatchFails) {
    Molecule pred, ref;
    std::string err;
    const auto p1 = write_temp("posebust_tiny2.sdf", kTinySdf);
    const auto p2 = write_temp("posebust_eth2.sdf", kEthaneSdf);
    ASSERT_TRUE(load_sdf(p1.string(), pred, &err));
    ASSERT_TRUE(load_sdf(p2.string(), ref, &err));
    EXPECT_FALSE(assign_topology_from_reference(pred, ref, &err));
    fs::remove(p1);
    fs::remove(p2);
}
