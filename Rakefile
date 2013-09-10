require "rubygems"
require 'rake/extensiontask'
require "hoe"

# Hoe.plugin :compiler
# Hoe.plugin :deveiate
# Hoe.plugin :gem_prelude_sucks
# Hoe.plugin :highline
# Hoe.plugin :inline
# Hoe.plugin :mercurial
# Hoe.plugin :racc
# Hoe.plugin :rcov
# Hoe.plugin :rubyforge

Hoe.spec "asn1" do
  developer("Daniel", "srcdaniel@gmail.com")

  self.extra_dev_deps << ['rake-compiler', '>= 0']
  self.spec_extras = { :extensions => ["ext/asn1/extconf.rb"] }

  license "MIT" # this should match the license in the README

  Rake::ExtensionTask.new('asn1', spec) do |ext|
    ext.lib_dir = File.join('lib', 'asn1')
  end
end
