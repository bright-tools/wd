require 'aruba/cucumber'

Before do
    home = Pathname.new(ENV.fetch('TMPDIR', '/tmp'))+"aruba_wb"
    ENV['HOME'] = home.to_s
    set_environment_variable 'HOME', home.to_s
    FileUtils.mkdir_p ENV['HOME']
end