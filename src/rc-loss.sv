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
    input majority,
    output exposed,
    output sync
);
    integer counter;
    reg[REGLEN - 1:0] register;

    assign exposed = register[0];
    assign sync = register[SYNCBITPOS];

    always @ (posedge reset) begin
        counter = 0;
        register = {REGLEN {1'b0}};
    end

    always @ (posedge clock) begin
        if (reset) begin
            register[REGLEN - 1] = seq[counter];
            counter += 1;
        end
    end

    wire rightmost;
    wire[REGLEN - 1:0] shifted;

    always @ (posedge clock) begin         
        if (majority == register[SYNCBITPOS] || reset) begin
            register = {register[REGLEN - 2:0], ^(register & FEEDBACK)};
        end
    end

endmodule