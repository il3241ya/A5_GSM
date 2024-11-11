module cipher #(
    parameter REG1LEN = 19,
    parameter REG2LEN = 22,
    parameter REG3LEN = 23,
    parameter MASK1 = 19'h07200,
    parameter MASK2 = 22'h300000,
    parameter MASK3 = 23'h700100,
    parameter SYNCBIT1 = 8,
    parameter SYNCBIT2 = 10,
    parameter SYNCBIT3 = 10,
    parameter FRAMENUMLEN = 22,
    parameter KEYLEN = 64,
    parameter CHUNKLEN = 114,
    parameter COUNTERLEN = 32
) (
    input in,
    input [FRAMENUMLEN + KEYLEN - 1:0] seq,
    input clock,
    input control,
    input reset,
    output reg out
);
    reg next_clock;

    wire F;
    wire gamma;
    wire outR1;
    wire outR2;
    wire outR3;
    wire syncR1;
    wire syncR2;
    wire syncR3;

    rcloss #(.FEEDBACK(MASK1), .REGLEN(REG1LEN), .SYNCBITPOS(SYNCBIT1)) loss1 (
        .seq(seq),
        .clock(next_clock),
        .reset(reset),
        .majority(F),
        .exposed(outR1),
        .sync(syncR1)
    );

    rcloss #(.FEEDBACK(MASK2), .REGLEN(REG2LEN), .SYNCBITPOS(SYNCBIT2)) loss2 (
        .seq(seq),
        .clock(next_clock),
        .reset(reset),
        .majority(F),
        .exposed(outR2),
        .sync(syncR2)
    );

    rcloss #(.FEEDBACK(MASK3), .REGLEN(REG3LEN), .SYNCBITPOS(SYNCBIT3)) loss3 (
        .seq(seq),
        .clock(next_clock),
        .reset(reset),
        .majority(F),
        .exposed(outR3),
        .sync(syncR3)
    );

    assign gamma = outR1 ^ outR2 ^ outR3;
    assign F = syncR1 & syncR2 | syncR1 & syncR3 | syncR2 & syncR3;

    always @ (clock) begin
        next_clock = 0;
    end

    always @ (clock) begin
        out = (control && !reset) ? in ^ gamma : 0;
    end

    always @ (clock) begin
        next_clock = clock;
    end

endmodule