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

  module Enumerize
    def enumerize(items)
      self.class_eval do
        const_set :SYM_TO_NUM, {}.tap{|h| items.map{|i| h[i[0].to_sym] = i[1]}}
        const_set :NUM_TO_SYM, {}.tap{|h| items.map{|i| h[i[1]] = i[0].to_sym}}

        def string=(s)
          self.sym=(s.to_sym)
        end

        def string
          sym.to_s
        end

        def sym=(sym)
          if self.class::SYM_TO_NUM.has_key? sym
            @value = self.class::SYM_TO_NUM[sym]
          else
            raise "Invalid selection #{sym.to_s} for enumeration"
          end
        end

        def sym
          self.class::NUM_TO_SYM[@value]
        end

        def value=(v)
          if self.class::NUM_TO_SYM.has_key?(v)
            @value = v
          else
            raise "Invalid selection #{sym.to_s} for enumeration"
          end
        end

        def value
          return @value
        end
      end
    end
  end

  class Choice
    extend Accessorize
    OPTIONS = ['one', 'two', 'three']

    accessorize(OPTIONS)
  end

  class Enumeration
    extend Enumerize
    ITEMS = [['one', 1], ['two', 2], ['three', 3]]
    enumerize ITEMS
  end
end

require 'asn1/asn1.so'
