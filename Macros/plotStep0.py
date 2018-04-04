import root_numpy
import matplotlib.pyplot as plt

if __name__ == "__main__":
    import sys
    import argparse

    parser.add_argument('-f', '--file1', required=True, help='File1 name')
    parser.add_argument('-k', '--file2', required=True, help='File2 name')
    parser.add_argument('-p', '--histogram', required=True, help='Histogram name to be plotted')

    args = parser.parse_args()
    file1=args.file1
    file2=args.file2
    histName=args.histogram

    rootFile1 = root_numpy.root2array(args.file1,branches=histogram)
    rootFile2 = root_numpy.root2array(args.file2,branches=histogram)

    plt.hist(rootfile1)
    plt.hist(rootfile2)
    #plt.savefig(histogram+'.pdf', bbox_inches='tight')
    plt.show()
