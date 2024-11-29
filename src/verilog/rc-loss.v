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
    output reg exposed,
    output reg sync
);
    integer counter;
    reg[REGLEN - 1:0] register;

    always @ (posedge clock) begin
        if (reset) begin
            if (counter == KEYLEN + FRAMENUMLEN - 1) begin
                register = {REGLEN {1'b0}};
                counter = 0;
            end
            register[0] = register[0] ^ seq[counter];
            counter = counter + 1;
            register = {register[REGLEN - 2:0], ^(register & FEEDBACK)};
        end else if (majority == register[SYNCBITPOS]) begin
            register = {register[REGLEN - 2:0], ^(register & FEEDBACK)};
        end
        exposed = register[REGLEN - 1];
        sync = register[SYNCBITPOS];
    end

    initial begin
        counter = 0;
        register = {REGLEN {1'b0}};
    end

endmodule