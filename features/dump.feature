Feature: dump

  Scenario: User dumps the bookmarks when the list file does not exist
    Given the default list file does not exist
    When I run wd with arguments "-d"
    Then the output should match:
    """
Dirlist has 0 entries of 100 used
    """
    And the exit status should be 0
    And stderr should match:
    """
Warning: Unable to load list file '(.+?)wb_list'\r*
Creating empty list
    """
    And the default list file should not exist

  # Ruby's chmod is a NOP in Windows
  @notwindows
  Scenario: User dumps the bookmarks when the list file has unfriendly permissions
    Given the default list file is empty
    Given the default list file is not readable
    When I run wd with arguments "-d"
    Then the output should match:
    """
Dirlist has 0 entries of 100 used
    """
    And the exit status should be 0
    And stderr should match:
    """
Warning: Unable to load list file '(.+?)wb_list'\r*
Creating empty list
    """