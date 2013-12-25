require 'singleton'

module Asn1
  VERSION = '1.0.0'

  class Undefined
    include Singleton
  end

  module Accessorize
    def accessorize(accessors)
      attr_accessor :asn1_choice_value

      accessors.each do |o|
        self.class_eval do
          define_method(o) do
            sym = "@#{o}".to_sym
            instance_variable_get sym
          end

          define_method(o + "=") do |x|
            sym = "@#{o}".to_sym
            instance_variable_set sym, x
            instance_variable_set :@asn1_choice_value, o.to_sym
          end
        end
      end
    end
  end

  class Choice
    extend Accessorize
    OPTIONS = ['one', 'two', 'three']

    accessorize(OPTIONS)
  end
end

require 'asn1/asn1.so'
