# Something

require 'rspec/expectations'

def get_default_file_list()
    return File.join(ENV['HOME'],".wb_list")
end

Given(/^the default list file does not exist$/) do
    remove(get_default_file_list(), :force => true)
end

Given(/^the default list file is empty$/) do
    write_file(get_default_file_list(), "")
end

Given(/^the default list file is not readable$/) do
    FileUtils.chmod_R(0000, get_default_file_list())
end

When(/I run wd with arguments "(.+?)"$/i) do |args|
    cmd = sanitize_text("src/wd -f "+get_default_file_list()+" " +args)
    run_simple(cmd, :fail_on_error => false)
end

Then(/the default list file should (not )?exist$/i) do |expect_match|
    file = get_default_file_list()
    if expect_match
        expect(file).not_to be_an_existing_file
    else
        expect(file).to be_an_existing_file
    end
end

Then(/the default list file should contain:$/) do |expected|
    file = get_default_file_list()
    expect(file).to have_file_content file_content_including(expected.chomp)
end

Then(/^stderr should( not)? match:$/) do |negated, expected|
    if negated
      expect(all_commands).not_to include_an_object have_output_on_stderr an_output_string_matching(expected)
    else
      expect(all_commands).to include_an_object have_output_on_stderr an_output_string_matching(expected)
    end
end

#     steps %Q{
#    a file named "#{file}" should exist
# }
