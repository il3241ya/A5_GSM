module rcloss #(
    parameter FEEDBACK = 0,
    parameter KEYLEN = 64, 
    parameter FRAMENUMLEN = 22,
    parameter REGLEN = 0,
    parameter SYNCBITPOS = 0
)(
    input [KEYLEN + FRAMENUMLEN - 1:0] seq,
    input clock,
    input reset,
    output exposed
);
    integer counter;
    reg[REGLEN - 1:0] register;

    assign exposed = register[0];

    always @ (posedge reset) begin
        counter = 0;
        register = {REGLEN {1'b0}};
    end

    always @ (posedge clock) begin
        if (reset) begin
            register[REGLEN - 1] = seq[counter];
        end
    end

    reg rightmost;
    reg[REGLEN:0] shifted;
    always @ (posedge clock) begin
        rightmost = ^(register & FEEDBACK) ^ 1;
        shifted = register << 1;
        register = {shifted[REGLEN - 1:1], rightmost};
    end

endmodule