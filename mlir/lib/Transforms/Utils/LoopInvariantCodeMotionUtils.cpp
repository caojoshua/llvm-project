//===- LoopInvariantCodeMotionUtils.cpp - LICM Utils ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the implementation of the core LICM algorithm.
//
//===----------------------------------------------------------------------===//

#include "mlir/Transforms/LoopInvariantCodeMotionUtils.h"
#include "mlir/IR/Operation.h"
#include "mlir/Interfaces/LoopLikeInterface.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "llvm/Support/Debug.h"
#include <queue>

#define DEBUG_TYPE "licm"

using namespace mlir;

size_t mlir::moveLoopInvariantCode(
    LoopLikeOpInterface loopLike,
    function_ref<bool(Value, Region *)> isDefinedOutsideRegion,
    function_ref<bool(Operation *, Region *)> shouldMoveOutOfRegion,
    function_ref<void(Operation *, Region *)> moveOutOfRegion) {
  size_t numMoved = 0;

  RegionRange regions = &loopLike.getLoopBody();
  for (Region *region : regions) {
    LLVM_DEBUG(llvm::dbgs() << "Original loop:\n"
                            << *region->getParentOp() << "\n");

    std::queue<Operation *> worklist;
    // Add top-level operations in the loop body to the worklist.
    for (Operation &op : region->getOps())
      worklist.push(&op);

    while (!worklist.empty()) {
      Operation *op = worklist.front();
      worklist.pop();
      // Skip ops that have already been moved. Check if the op can be hoisted.
      if (op->getParentRegion() != region)
        continue;

      LLVM_DEBUG(llvm::dbgs() << "Checking op: " << *op << "\n");
      llvm::outs() << "is spec: " << isSpeculatable(op) << "\n";
      llvm::outs() << "is mem: " << isMemoryEffectFree(op) << "\n";
      if (!shouldMoveOutOfRegion(op, region) || !loopLike.isLoopInvariant(op))
        continue;

      LLVM_DEBUG(llvm::dbgs() << "Moving loop-invariant op: " << *op << "\n");
      moveOutOfRegion(op, region);
      ++numMoved;

      // Since the op has been moved, we need to check its users within the
      // top-level of the loop body.
      for (Operation *user : op->getUsers())
        if (user->getParentRegion() == region)
          worklist.push(user);
    }
  }

  return numMoved;
}

size_t mlir::moveLoopInvariantCode(LoopLikeOpInterface loopLike) {
  return moveLoopInvariantCode(
      loopLike,
      [&](Value value, Region *) {
        return loopLike.isDefinedOutsideOfLoop(value);
      },
      [&](Operation *op, Region *) {
        // Would really like to move this logic to `isLoopInvariant`...
        return isMemoryEffectFree(op) && isSpeculatable(op) && !op->hasTrait<OpTrait::IsTerminator>();
      },
      [&](Operation *op, Region *) { loopLike.moveOutOfLoop(op); });
}
