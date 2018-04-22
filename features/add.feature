Feature: add

#  Scenario: User adds a bookmark to a previously non-existing bookmark file
#    Given a file named "test" with:
#"""
#:C:\
#N:see
#"""
#    Then the file named "test" should match /:C:\\\n(N:.*)?/

  Scenario Outline: User adds a bookmark with no pre-existing bookmarks file
    Given the default list file does not exist
    When I run wd with arguments "-z 1386181003 -a '<dir>' "<bookmark>""
    Then the output should match:
    """
    """
    And the exit status should be 0
    And stderr should match:
    """
Warning: Unable to load list file '(.+?)wb_list'\r*
Creating empty list
    """
    And the default list file should exist
    And the default list file should contain a header
    And the default list file should contain 1 shortcut
    And the default list file should contain a shortcut to '<dir>'
    And the default list file should contain a shortcut to unknown '<dir>'
    And the default list file should contain a shortcut to unknown '<dir>' named "<bookmark>"
    And the default list file should contain a shortcut to unknown '<dir>' named "<bookmark>" with timestamp "2013/12/04 18:16:43"

    @notwindows
    Examples:
      | dir           | bookmark |
      | /doesnt_exist | root     |
    @onlywindows
    Examples:
      | dir   | bookmark |
      | c:\\a | see      |

  @onlywindows
  Scenario: User attempts to add adds a bookmark with a duplicate name
    Given the default list file does not exist
    When I run wd with arguments "-z 1386181003 -a 'C:\\a' see"
    Then the default list file should contain a shortcut to 'C:\a' named "see" with timestamp "2013/12/04 18:16:43"
    And I run wd with arguments "-z 1386181009 -a 'C:\\b' see"
    Then the default list file should contain 1 shortcut
    And the default list file should contain a shortcut to 'C:\a' named "see" with timestamp "2013/12/04 18:16:43"

  @onlywindows
  Scenario: User attempts to add adds a bookmark with a duplicate directory
    Given the default list file does not exist
    When I run wd with arguments "-z 1386181003 -a 'C:\\a' see"
    And I run wd with arguments "-z 1386181009 -a 'C:\\a' vee"
    Then the default list file should contain 1 shortcut
    And the default list file should contain a shortcut to 'C:\a' named "see" with timestamp "2013/12/04 18:16:43"

  @onlywindows
  Scenario: User attempts to add adds a second bookmark
    Given the default list file does not exist
    When I run wd with arguments "-z 1386181003 -a 'C:\\a' see"
    And I run wd with arguments "-z 1386181009 -a 'C:\\b' bee"
    Then the default list file should contain 2 shortcut
    And the default list file should contain a shortcut to 'C:\a' named "see" with timestamp "2013/12/04 18:16:43"
    And the default list file should contain a shortcut to 'C:\b' named "bee" with timestamp "2013/12/04 18:16:49"