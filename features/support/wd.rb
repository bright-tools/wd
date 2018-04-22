# Something

require 'rspec/expectations'

def get_default_file_list()
    return File.join(ENV['HOME'],".wb_list")
end

Given(/^the default list file does not exist$/) do
    file = get_default_file_list()
    remove(file, :force => true)
    step "a file named \"#{file}\" should not exist"
end

Given(/^the default list file is empty$/) do
    write_file(get_default_file_list(), "")
end

Given(/^the default list file is not readable$/) do
    FileUtils.chmod_R(0000, get_default_file_list())
end

When(/I run wd with arguments "(.+?)"$/i) do |args|
    cmd = sanitize_text("src/wd -f "+get_default_file_list()+" " +args)
#    print "Running: #{cmd}\n" 
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

Then(/the default list file should contain a header$/) do
    file = get_default_file_list()
    expected = "# WD directory list file\n" +
               "# File format: version 1\n"
    expect(file).to have_file_content file_content_including(expected.chomp)
end

Then(/the default list file should contain ([0-9]+?) shortcut(?:s)?/) do |expect_count|
    # TODO: Bit of a long-winded, high-overhead way of achieving this instead of counting the matches

    # Create a match string based on the expected number of shortcuts
    shortcut_match = "^:.*\n.*"
    expected = shortcut_match * Integer(expect_count)

    # Check to see that the file content includes the number of shortcuts that we expect
    file = get_default_file_list()
    expect(file).to have_file_content file_content_matching(expected)

    # Add an additional shortcut matcher & check that this no longer matches
    # i.e. we match expected_count, but not expected_count+1 times
    expected += shortcut_match
    expect(file).not_to have_file_content file_content_matching(expected)
end

Then(/the default list file should contain a shortcut to(?: (unknown|directory|file)?)? '([^"]*)'( named "([^"]*)")?( with timestamp "(.+?)")?$/) do |file_or_directory, shortcut, named, timestamp|
    expected = ":"+Shellwords.escape(shortcut)+"\n"

    if named
        expected += "N:#{named}\n"
    else
        expected += "(N:.*)?"
    end

    if timestamp
        expected += "A:#{timestamp}\n"
    else
        expected += "(A:.*)?"
    end

    if file_or_directory == 'file'
        expected += "T:F"
    elsif file_or_directory == 'directory'
        expected += "T:D"
    elsif file_or_directory == 'unknown'
        expected += "T:U"
    else
        expected += "(T:.*)"
    end

#    print "XX: '#{shortcut}'\nYY: '#{file_or_directory}'\nII: '#{named}'\nTT: '#{timestamp}'\nOO: '#{expected}'"
#    print "---\n"
#    file = File.open(get_default_file_list(), "r")
#    contents = file.read
#    file.close
#    print contents
#    print "---\n"

    file = get_default_file_list()
    expect(file).to have_file_content file_content_matching(expected)
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
