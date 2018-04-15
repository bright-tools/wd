require 'cucumber/core/filter'
require 'rbconfig'

# Filter out tests which don't run on Windows
WindowsFilter = Cucumber::Core::Filter.new do
    def test_case(test_case)
        is_not_windows = !(RbConfig::CONFIG['host_os'] =~ /mswin|mingw|cygwin/i)
        is_not_tagged = !test_case.match_tags?("@notwindows")

        if is_not_windows || is_not_tagged then
            test_case.describe_to(receiver)
        end
    end
end

AfterConfiguration do |config|
  config.filters << WindowsFilter.new
end