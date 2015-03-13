using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;

namespace Aga.Controls.Tree.NodeControls
{
    public class NodeSizeControl : NodeControl
    {
        private readonly Size _fixedSize;
        private readonly IList<Type> _nodeTypesThatShouldBeSized;
        private readonly Size _firstRowSize;

        public NodeSizeControl(Size fixedSize, IList<Type> nodeTypesThatShouldBeSized)
        {
            _fixedSize = fixedSize;
            _nodeTypesThatShouldBeSized = nodeTypesThatShouldBeSized;
        }

        public NodeSizeControl(Size fixedSize, Size firstRowSize, IList<Type> nodeTypesThatShouldBeSized)
        {
            _fixedSize = fixedSize;
            _nodeTypesThatShouldBeSized = nodeTypesThatShouldBeSized;
            _firstRowSize = firstRowSize;
        }

        public override Size MeasureSize(TreeNodeAdv node, DrawContext context)
        {
            if (!(node.Tag is Node)) return new Size(0, 0);

            var properNode = (Node) node.Tag;

            if (!_nodeTypesThatShouldBeSized.Contains(properNode.GetType())) return new Size(0, 0);

            if (node.Row == 0 && _firstRowSize != default(Size)) return _firstRowSize;

            return _fixedSize;
        }

        private Size MeasureSizeOfFirstRow(TreeNodeAdv node, DrawContext context)
        {
            return _firstRowSize;
        }

        public override void Draw(TreeNodeAdv node, DrawContext context)
        {
            
        }
    }
}
