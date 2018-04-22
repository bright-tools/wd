require 'cucumber/core/filter'
require 'rbconfig'

# Filter out tests which don't run on Windows
WindowsFilter = Cucumber::Core::Filter.new do
    def test_case(test_case)
        is_not_windows_platform = !(RbConfig::CONFIG['host_os'] =~ /mswin|mingw|cygwin/i)
        is_not_windows_test = !test_case.match_tags?("@notwindows")
        is_windows_only_test = test_case.match_tags?("@onlywindows")

        if is_not_windows_platform && is_windows_only_test then
            # 
        elsif is_not_windows_platform || is_not_windows_test then
            test_case.describe_to(receiver)
        end
    end
end

AfterConfiguration do |config|
  config.filters << WindowsFilter.new
end