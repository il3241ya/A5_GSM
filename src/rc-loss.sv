// defines register filler, to initilize empty registers and
// set them up for iteration
module initilize #(
    parameter FEEDBACK = 0,
    parameter KEYLEN = 64, 
    parameter FRAMENUMLEN = 22, 
    parameter REGLEN = 24,
    parameter COUNTERLEN = 32
) (
    input [COUNTERLEN - 1:0] current,
    input [REGLEN - 1:0] register_in,
    input [KEYLEN + REGLEN - 1:0] seq,
    input clock,
    output reg [REGLEN - 1:0] register_out
);
    // renew register & save & shift
    reg rightmost;
    // all feedback masks should have leftmost bit in them,
    // or else this won't work
    always @ (posedge clock) begin
        register_out = register_in;
    end

    generate
        genvar i;
        for (i = 0; i < KEYLEN + REGLEN; i = i + 1) begin : lolkek
            always @ (posedge clock) begin
                if (current == i) begin 
                    register_out[REGLEN - 1] = seq[i];
                end
            end 
        end
    endgenerate

    always @ (posedge clock) begin
        rightmost = ^(register_out & FEEDBACK);
        register_out = register_out << 1;
        register_out[0] = rightmost;
    end 

endmodule

module rcloss #(
    parameter FEEDBACK = 0,
    parameter REGLEN = 24,
    parameter SYNCBITPOS = 10
)(
    input [REGLEN - 1:0] register_in,
    input F,
    input clock,
    output reg [REGLEN - 1:0] register_out
);
    reg rightmost;
    always @ (posedge clock) begin
        if (register_in[SYNCBITPOS] == F) begin
            rightmost = ^(register_in & FEEDBACK);
            register_out = register_in << 1;
            register_out[0] = rightmost;
        end
    end
endmodule