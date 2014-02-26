require 'spec_helper'
require 'asn1'

describe "Asn1::Type::Integer" do
  it "encodes an integer" do
    Asn1::Type::Integer.encode(:xer, 1).should == "<INTEGER>1</INTEGER>"
  end

  it "decodes an integer" do
    Asn1::Type::Integer.decode(:xer, "<INTEGER>1</INTEGER>").should == 1
  end
end
