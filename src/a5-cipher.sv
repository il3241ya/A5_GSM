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
    parameter CHUNKLEN = 114,
    parameter COUNTERLEN = 32
) (
    input [COUNTERLEN - 1:0] current, 
    input [REG1LEN - 1:0] R1,
    input [REG2LEN - 1:0] R2,
    input [REG3LEN - 1:0] R3,
    input [CHUNKLEN - 1:0] in,
    input clock,
    input control,
    output reg [CHUNKLEN - 1:0] out
);
    wire gamma;
    reg next_clock;
    wire F;

    rcloss #(.FEEDBACK(MASK1), .REGLEN(REG1LEN), .SYNCBITPOS(SYNCBIT1)) loss1 (
        .register_in(R1),
        .F(F),
        .clock(next_clock),
        .register_out(R1)
    );

    rcloss #(.FEEDBACK(MASK2), .REGLEN(REG2LEN), .SYNCBITPOS(SYNCBIT2)) loss2 (
        .register_in(R2),
        .F(F),
        .clock(next_clock),
        .register_out(R2)
    );

    rcloss #(.FEEDBACK(MASK3), .REGLEN(REG3LEN), .SYNCBITPOS(SYNCBIT3)) loss3 (
        .register_in(R3),
        .F(F),
        .clock(next_clock),
        .register_out(R3)
    );

    assign gamma = R1[REG1LEN - 1] ^ R2[REG2LEN - 1] ^ R3[REG3LEN - 1];
    assign F = R1[SYNCBIT1] & R2[SYNCBIT2] | R1[SYNCBIT1] & R3[SYNCBIT3] | R2[SYNCBIT2] & R3[SYNCBIT3];

    always @ (clock) begin
        next_clock = 0;
    end

    generate
        genvar i;
        for (i = 0; i < CHUNKLEN; i = i + 1) begin
            always @ (clock) begin
                if (current == i && control) begin
                    out[i] = in[i] ^ gamma;
                end
            end
        end
    endgenerate
    

    always @ (clock) begin
        next_clock = clock;
    end

endmodule