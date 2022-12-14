/* rv64-emu -- Simple 64-bit RISC-V simulator
 *
 *    stages.cc - Pipeline stages
 *
 * Copyright (C) 2016-2020  Leiden University, The Netherlands.
 */

#include "stages.h"

#include <iostream>


/*
 * Instruction fetch
 */

void
InstructionFetchStage::propagate()
{
  try {
    instructionMemory.setAddress(PC);
    instructionMemory.setSize(4);

      if (instructionMemory.getValue() == TestEndMarker)
        throw TestEndMarkerEncountered(PC);
    }
  catch (TestEndMarkerEncountered &e)
    {
      throw;
    }
  catch (std::exception &e)
    {
      throw InstructionFetchFailure(PC);
    }
}

void
InstructionFetchStage::clockPulse()
{
  if_id.PC = PC;
  if_id.instructionWord = instructionMemory.getValue();
}

/*
 * Instruction decode
 */

void
dump_instruction(std::ostream &os, const uint32_t instructionWord,
                 const InstructionDecoder &decoder);

void
InstructionDecodeStage::propagate()
{
  decoder.setInstructionWord(if_id.instructionWord);


  //fields goed zetten en met enums bepalen welke instructie en wlke fields preciesTODO

  /* TODO: need a control signals class that generates control
   * signals from a given opcode and function code.

   dit hieronder gebeurt in die class decideinstuction
dan voor switch decideinstruction doen we registers invullen en immediates bepalen

decode.calcOP();
opcode = decode.opcode() //is gonne be an enum
funct3 = decode.getfunct3()
if switch(opcode)
    switch(funct3)
      boeie
      oke nu weet je dat het een add is of iets met gelijke waarde

   */

  PC = if_id.PC;


  /* debug mode: dump decoded instructions to cerr.
   * In case of no pipelining: always dump.
   * In case of pipelining: special case, if the PC == 0x0 (so on the
   * first cycle), don't dump an instruction. This avoids dumping a
   * dummy instruction on the first cycle when ID is effectively running
   * uninitialized.
   */
  if (debugMode && (! pipelining || (pipelining && PC != 0x0)))
    {
      /* Dump program counter & decoded instruction in debug mode */
      auto storeFlags(std::cerr.flags());

      std::cerr << std::hex << std::showbase << PC << "\t";
      std::cerr.setf(storeFlags);

      std::cerr << decoder << std::endl;
    }

  /* TODO: register fetch */
}

void InstructionDecodeStage::clockPulse()
{
  /* ignore the "instruction" in the first cycle. */
  if (! pipelining || (pipelining && PC != 0x0))
    ++nInstrIssued;

  /* TODO: write necessary fields in pipeline register */
  id_ex.PC = PC;
}

/*
 * Execute
 */

void
ExecuteStage::propagate()
{
  /* TODO configure ALU based on control signals and using inputs
   * from pipeline register.
   * Consider using the Mux class.
   */

  PC = id_ex.PC;
}

void
ExecuteStage::clockPulse()
{
  /* TODO: write necessary fields in pipeline register. This
   * includes the result (output) of the ALU. For memory-operations
   * the ALU computes the effective memory address.
   */

  ex_m.PC = PC;
}

/*
 * Memory
 */

void
MemoryStage::propagate()
{
  /* TODO: configure data memory based on control signals and using
   * inputs from pipeline register.
   */


  /* TODO: perhaps also determine and write the new PC here. */
  PC = ex_m.PC;
}

void
MemoryStage::clockPulse()
{
  /* TODO: pulse the data memory */

  /* TODO: write necessary fields in pipeline register */
  m_wb.PC = PC;
}

/*
 * Write back
 */

void
WriteBackStage::propagate()
{
  if (! pipelining || (pipelining && m_wb.PC != 0x0))
    ++nInstrCompleted;

  /* TODO: configure write lines of register file based on control
   * signals
   */
}

void
WriteBackStage::clockPulse()
{
  /* TODO: pulse the register file */
}
