module register(clock, reset, output_enable, load, data_in, data_out);
	parameter DATA_WIDTH = 16;
	input clock, reset, output_enable, load;
	input [DATA_WIDTH-1:0] data_in;
	output [DATA_WIDTH-1:0] data_out;
	
	wire clock, reset, output_enable, load;
	wire [DATA_WIDTH-1:0] data_in;
	wire [DATA_WIDTH-1:0] data_out;
	
	reg [DATA_WIDTH-1:0] data;
	
	assign data_out = output_enable ? data : 32'bzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz;
	
	always @ (posedge clock)
	begin
		if(reset) begin
			data = 0;
		end	else begin
			if(load) begin
				data = data_in;
			end
		end
	
	end
	
endmodule
