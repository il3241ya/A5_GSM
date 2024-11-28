`timescale 1ns / 100ps

module testbench;

    parameter REG1LEN = 19;
    parameter REG2LEN = 22;
    parameter REG3LEN = 23;
    parameter CHUNKLEN = 114;

    parameter MASK1 = 19'h72000;
    parameter MASK2 = 22'h300000;
    parameter MASK3 = 23'h700100;

    parameter KEYLEN = 64;
    parameter FRAMELEN = 22;

    integer j = 0;

    // control units
    reg clock = 0;
    reg control = 0;
    reg reset = 0;

    // input and output data
    reg i;
    wire o;
    reg[CHUNKLEN - 1:0] out = {CHUNKLEN {1'b0}};
    reg[CHUNKLEN - 1:0] in = {CHUNKLEN {1'b0}};

    // key & frame (suppose 2)
    reg[KEYLEN - 1:0] key = 64'h1223456789ABCDEF;
    reg[FRAMELEN - 1:0] frame = 22'h2;

    cipher #(
        .REG1LEN(REG1LEN),
        .REG2LEN(REG2LEN),
        .REG3LEN(REG3LEN),
        .CHUNKLEN(CHUNKLEN),
        .MASK1(MASK1),
        .MASK2(MASK2),
        .MASK3(MASK3)
    ) uut (
        .in(i),
        .seq({key, frame}),
        .clock(clock),
        .control(control),
        .reset(reset),
        .out(o)
    );

    parameter INITRUN = 86;
    parameter DRYRUN = 100;

    initial begin

        // init run (86 ticks)
        reset = 1;
        for (j = 0; j < INITRUN; j = j + 1) begin
            clock = 1;
            #5;
            clock = 0;
        end

        // dry run
        reset = 0;
        for (j = 0; j < DRYRUN; j = j + 1) begin
            clock = 1;
            #5;
            clock = 0;
        end

        // actual cypher
        control = 1;
        for (j = 0; j < CHUNKLEN; j = j + 1) begin
            $display("running iteration: bit in: %d", in[j]);
            i = in[j];
            clock = 1;
            #5;
            $display("bit out: %d", o);
            out[j] = o;
            clock = 0;
        end

        $display("0x%01h_%04h_%04h_%04h_%04h_%04h_%04h_%04h", out[113:110], out[109:96], out[95:80], out[79:64], out[63:48], out[47:32], out[31:16], out[15:0]);
    end

    initial
        $dumpvars;

endmodule