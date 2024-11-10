`timescale 1ns / 100ps

module testbench;

    parameter REG1LEN = 19;
    parameter REG2LEN = 22;
    parameter REG3LEN = 23;
    parameter CHUNKLEN = 114;

    parameter MASK1 = 19'h07200;
    parameter MASK2 = 22'h300000;
    parameter MASK3 = 23'h700100;

    parameter KEYLEN = 64;
    parameter FRAMELEN = 22;

    // declare state (registers)
    reg[REG1LEN - 1:0] R1 = {REG1LEN {1'b0}};
    reg[REG2LEN - 1:0] R2 = {REG2LEN {1'b0}};
    reg[REG3LEN - 1:0] R3 = {REG3LEN {1'b0}};

    // control units
    reg clock = 0;
    reg actual_clock = 0;
    reg control = 0;
    integer current = 0;

    // input and output data
    reg[CHUNKLEN - 1:0] out;
    reg[CHUNKLEN - 1:0] in;

    // key & frame (suppose 2)
    // reg[KEYLEN - 1:0] key = 64'h1223456789ABCDEF;
    // reg[FRAMELEN - 1:0] frame = 22'h2;
    reg[KEYLEN - 1:0] key;
    reg[FRAMELEN - 1:0] frame;

    cipher #(
        .REG1LEN(REG1LEN),
        .REG2LEN(REG2LEN),
        .REG3LEN(REG3LEN),
        .CHUNKLEN(CHUNKLEN),
        .MASK1(MASK1),
        .MASK2(MASK2),
        .MASK3(MASK3)
    ) uut (
        .current(current), 
        .R1(R1),
        .R2(R2),
        .R3(R3),
        .in(in),
        .clock(actual_clock),
        .control(control),
        .out(out)
    );

    initilize #(
        .FEEDBACK(MASK1),
        .KEYLEN(KEYLEN), 
        .FRAMENUMLEN(FRAMELEN), 
        .REGLEN(REG1LEN)
    ) R1Init (
        .current(current),
        .register_in(R1),
        .seq({key, frame}),
        .clock(clock),
        .register_out(R1)
    );

    initilize #(
        .FEEDBACK(MASK2),
        .KEYLEN(KEYLEN), 
        .FRAMENUMLEN(FRAMELEN), 
        .REGLEN(REG2LEN)
    ) R2Init (
        .current(current),
        .register_in(R2),
        .seq({key, frame}),
        .clock(clock),
        .register_out(R2)
    );

    initilize #(
        .FEEDBACK(MASK3),
        .KEYLEN(KEYLEN), 
        .FRAMENUMLEN(FRAMELEN), 
        .REGLEN(REG3LEN)
    ) R3Init (
        .current(current),
        .register_in(R3),
        .seq({key, frame}),
        .clock(clock),
        .register_out(R3)
    );

    parameter INITRUN = 86;
    parameter DRYRUN = 100;

    initial begin
        // initilize state (64 + 22 ticks = 86 total)
        forever begin
            if (current == INITRUN) begin
                $break;
            end
            clock = 0;
            current = current + 1;
            clock = 1;
            #1;
        end

        clock = 0;
        current = 0;
        // just in case
        control = 0;
        // rcloss dry run (100 ticks)
        forever begin
            if (current == DRYRUN) begin
                $break;
            end
            actual_clock = 0;
            current = current + 1;
            actual_clock = 1;
            #1;
        end

        clock = 0;
        current = 0;
        actual_clock = 1;
        forever begin
            if (current == CHUNKLEN) begin
                $break;
            end
            actual_clock = 0;
            current = current + 1;
            actual_clock = 1;
            #1;
        end
    end

    // initial begin
    //     forever begin
    //         #1;
    //         if (product != extended) begin
    //             $display("assertion failed: %d (m) * %d (r) != %d (m * r), got instead: %d (product)", m, r, extended, product);
    //             $display("binary: %b (m * r), %b (product)", extended, product);
    //             $stop;
    //         end else begin
    //             $display("assertion passed: %d (m) * %d (r) == %d (product)", m, r, product);
    //         end
    //         m = $urandom_range(-2 ** (WIDTH-1), 2 ** (WIDTH-1) - 1);
    //         r = $urandom_range(-2 ** (WIDTH-1), 2 ** (WIDTH-1) - 1);
    //     end
    // end

    initial
        $dumpvars;

endmodule